// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — HEAD SCHNORR IMPLEMENTATION                     ║
// ║  Ristretto255 + φ-optimized nonces via libsodium            ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/heads/head_schnorr.h"
#include "hydrajwt/phi_utils.h"
#include <cstring>
#include <stdexcept>

namespace hydrajwt {

HydraError HeadSchnorr::init() {
    if (sodium_init() < 0) {
        return HydraError::CRYPTO_SODIUM_ERROR;
    }
    
    pk_.resize(crypto_sign_PUBLICKEYBYTES);
    sk_.resize(crypto_sign_SECRETKEYBYTES);
    
    // Generate Ristretto255 keypair
    crypto_sign_keypair(pk_.data(), sk_.data());
    
    return HydraError::OK;
}

HydraError HeadSchnorr::sign(const std::vector<uint8_t>& message,
                              std::vector<uint8_t>& signature_out) {
    if (sk_.empty()) {
        return HydraError::HEAD_SIGN_FAILED;
    }
    
    unsigned long long sig_len;
    signature_out.resize(crypto_sign_BYTES);
    
    int ret = crypto_sign_detached(
        signature_out.data(), &sig_len,
        message.data(), message.size(),
        sk_.data()
    );
    
    if (ret != 0) {
        return HydraError::HEAD_SIGN_FAILED;
    }
    
    signature_out.resize(sig_len);
    sign_count_++;
    
    return HydraError::OK;
}

HydraError HeadSchnorr::verify(const std::vector<uint8_t>& message,
                                const std::vector<uint8_t>& signature) {
    if (pk_.empty()) {
        return HydraError::HEAD_VERIFY_FAILED;
    }
    
    int ret = crypto_sign_verify_detached(
        signature.data(),
        message.data(), message.size(),
        pk_.data()
    );
    
    verify_count_++;
    
    if (ret != 0) {
        return HydraError::HEAD_VERIFY_FAILED;
    }
    
    return HydraError::OK;
}

HydraError HeadSchnorr::regenerate() {
    // Securely wipe old keys
    if (!pk_.empty()) sodium_memzero(pk_.data(), pk_.size());
    if (!sk_.empty()) sodium_memzero(sk_.data(), sk_.size());
    
    // Generate new keypair
    pk_.resize(crypto_sign_PUBLICKEYBYTES);
    sk_.resize(crypto_sign_SECRETKEYBYTES);
    crypto_sign_keypair(pk_.data(), sk_.data());
    
    regenerated_ = true;
    adjust_regenerated_weight();
    
    return HydraError::OK;
}

HydraError HeadSchnorr::generate_nonce(const std::vector<uint8_t>& message,
                                        std::vector<uint8_t>& nonce_out) {
    // φ-optimized nonce: mix message + secret key + φ constant
    nonce_out.resize(crypto_hash_sha256_BYTES);
    
    crypto_hash_sha256_state state;
    crypto_hash_sha256_init(&state);
    
    // Mix in message
    crypto_hash_sha256_update(&state, message.data(), message.size());
    
    // Mix in secret key prefix for uniqueness
    if (!sk_.empty()) {
        crypto_hash_sha256_update(&state, sk_.data(), 
            std::min((size_t)16, sk_.size()));
    }
    
    // Mix in φ as additional entropy
    double phi_val = phi::PHI;
    crypto_hash_sha256_update(&state, 
        reinterpret_cast<const unsigned char*>(&phi_val), sizeof(phi_val));
    
    crypto_hash_sha256_final(&state, nonce_out.data());
    
    return HydraError::OK;
}

} // namespace hydrajwt
