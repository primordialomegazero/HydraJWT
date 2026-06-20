// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — HEAD ECDSA P-256 IMPL                           ║
// ║  Legacy compat via OpenSSL                                   ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/heads/head_ecdsa.h"
#include <cstring>

namespace hydrajwt {

HeadECDSA::~HeadECDSA() {
    if (key_) EVP_PKEY_free(key_);
}

HydraError HeadECDSA::init() {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx || EVP_PKEY_keygen_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0 ||
        EVP_PKEY_keygen(ctx, &key_) <= 0) {
        if (ctx) EVP_PKEY_CTX_free(ctx);
        return HydraError::CRYPTO_KEYGEN_FAILED;
    }
    EVP_PKEY_CTX_free(ctx);
    return HydraError::OK;
}

HydraError HeadECDSA::sign(const std::vector<uint8_t>& message,
                            std::vector<uint8_t>& signature_out) {
    if (!key_) return HydraError::HEAD_SIGN_FAILED;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx || EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, key_) <= 0) {
        if (ctx) EVP_MD_CTX_free(ctx);
        return HydraError::HEAD_SIGN_FAILED;
    }
    size_t sig_len;
    EVP_DigestSign(ctx, nullptr, &sig_len, message.data(), message.size());
    signature_out.resize(sig_len);
    if (EVP_DigestSign(ctx, signature_out.data(), &sig_len, message.data(), message.size()) <= 0) {
        EVP_MD_CTX_free(ctx);
        return HydraError::HEAD_SIGN_FAILED;
    }
    signature_out.resize(sig_len);
    EVP_MD_CTX_free(ctx);
    sign_count_++;
    return HydraError::OK;
}

HydraError HeadECDSA::verify(const std::vector<uint8_t>& message,
                              const std::vector<uint8_t>& signature) {
    if (!key_) return HydraError::HEAD_VERIFY_FAILED;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx || EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, key_) <= 0) {
        if (ctx) EVP_MD_CTX_free(ctx);
        return HydraError::HEAD_VERIFY_FAILED;
    }
    verify_count_++;
    int ret = EVP_DigestVerify(ctx, signature.data(), signature.size(),
                               message.data(), message.size());
    EVP_MD_CTX_free(ctx);
    return (ret == 1) ? HydraError::OK : HydraError::HEAD_VERIFY_FAILED;
}

HydraError HeadECDSA::regenerate() {
    if (key_) EVP_PKEY_free(key_);
    key_ = nullptr;
    regenerated_ = true;
    adjust_regenerated_weight();
    return init();
}

std::vector<uint8_t> HeadECDSA::public_key_bytes() const {
    if (!key_) return {};
    size_t len;
    EVP_PKEY_get_octet_string_param(key_, "pub", nullptr, 0, &len);
    std::vector<uint8_t> pk(len);
    EVP_PKEY_get_octet_string_param(key_, "pub", pk.data(), len, &len);
    return pk;
}

std::vector<uint8_t> HeadECDSA::secret_key_bytes() const {
    return {};
}

} // namespace hydrajwt
