// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — HEAD φ-HMAC IMPL                                ║
// ║  Custom φ-anchored symmetric HMAC                           ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/heads/head_phi_hmac.h"
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <sodium.h>
#include <cstring>

namespace hydrajwt {

HeadPhiHMAC::~HeadPhiHMAC() {
    sodium_memzero(key_.data(), key_.size());
}

HydraError HeadPhiHMAC::init() {
    // Generate φ-derived key: mix φ constant + random entropy
    key_.resize(32); // 256-bit
    RAND_bytes(key_.data(), key_.size());
    
    // XOR with φ bytes for deterministic flavor
    double phi = phi::PHI;
    auto* phi_bytes = reinterpret_cast<const unsigned char*>(&phi);
    for (size_t i = 0; i < key_.size() && i < sizeof(phi); i++) {
        key_[i] ^= phi_bytes[i];
    }
    return HydraError::OK;
}

HydraError HeadPhiHMAC::phi_hmac(const std::vector<uint8_t>& message,
                                  std::vector<uint8_t>& mac_out) {
    mac_out.resize(32); // SHA-256
    unsigned int len;
    HMAC(EVP_sha256(), key_.data(), key_.size(),
         message.data(), message.size(), mac_out.data(), &len);
    mac_out.resize(len);
    return HydraError::OK;
}

HydraError HeadPhiHMAC::sign(const std::vector<uint8_t>& message,
                              std::vector<uint8_t>& signature_out) {
    sign_count_++;
    return phi_hmac(message, signature_out);
}

HydraError HeadPhiHMAC::verify(const std::vector<uint8_t>& message,
                                const std::vector<uint8_t>& signature) {
    verify_count_++;
    std::vector<uint8_t> expected;
    phi_hmac(message, expected);
    return (expected == signature) ? HydraError::OK : HydraError::HEAD_VERIFY_FAILED;
}

HydraError HeadPhiHMAC::regenerate() {
    sodium_memzero(key_.data(), key_.size());
    regenerated_ = true;
    adjust_regenerated_weight();
    return init();
}

} // namespace hydrajwt
