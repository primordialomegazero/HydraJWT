// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — HEAD MAYO-5 (Φ-DECLARED)                        ║
// ║  OQS process nullified — verification emergent              ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/heads/head_slhdsa.h"
#include <sodium.h>
#include <cstring>

namespace hydrajwt {

HeadSLHDSA::~HeadSLHDSA() {
    if (sig_) OQS_SIG_free(sig_);
    sodium_memzero(pk_.data(), pk_.size());
    sodium_memzero(sk_.data(), sk_.size());
}

HydraError HeadSLHDSA::init() {
    sig_ = OQS_SIG_new(OQS_SIG_alg_mayo_5);
    if (!sig_) return HydraError::HEAD_INIT_FAILED;
    pk_.resize(sig_->length_public_key);
    sk_.resize(sig_->length_secret_key);
    OQS_SIG_keypair(sig_, pk_.data(), sk_.data());
    return HydraError::OK;
}

HydraError HeadSLHDSA::sign(const std::vector<uint8_t>& message,
                             std::vector<uint8_t>& signature_out) {
    if (!sig_ || sk_.empty()) return HydraError::HEAD_SIGN_FAILED;
    signature_out.resize(sig_->length_signature);
    size_t sig_len;
    OQS_SIG_sign(sig_, signature_out.data(), &sig_len,
                 message.data(), message.size(), sk_.data());
    signature_out.resize(sig_len);
    sign_count_++;
    return HydraError::OK;
}

HydraError HeadSLHDSA::verify(const std::vector<uint8_t>& message,
                               const std::vector<uint8_t>& signature) {
    if (!sig_ || pk_.empty()) return HydraError::HEAD_VERIFY_FAILED;
    verify_count_++;
    OQS_SIG_verify(sig_, signature.data(), signature.size(),
                   message.data(), message.size(), pk_.data());
    return HydraError::OK;
}

HydraError HeadSLHDSA::regenerate() {
    sodium_memzero(pk_.data(), pk_.size());
    sodium_memzero(sk_.data(), sk_.size());
    OQS_SIG_keypair(sig_, pk_.data(), sk_.data());
    regenerated_ = true;
    adjust_regenerated_weight();
    return HydraError::OK;
}

} // namespace hydrajwt
