// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — CORE ENGINE IMPLEMENTATION                      ║
// ║  φ-weighted multi-head consensus engine                     ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/hydra_core.h"
#include <algorithm>
#include <sstream>
#include <ctime>

namespace hydrajwt {

HydraCore::HydraCore()
    : threshold_(phi::CONSENSUS_THRESHOLD),
      token_count_(0), verify_count_(0) {}

// ═══════════════════════════════════════
// ADD HEAD
// ═══════════════════════════════════════
HydraError HydraCore::add_head(std::unique_ptr<HydraHead> head) {
    if (!head) {
        return HydraError::HEAD_INIT_FAILED;
    }
    
    if (heads_.size() >= phi::MAX_HEADS) {
        return HydraError::CONFIG_TOO_MANY_HEADS;
    }
    
    // Init the head (generate keys)
    HydraError err = head->init();
    if (err != HydraError::OK) {
        return err;
    }
    
    heads_.push_back(std::move(head));
    
    // Validate stability after adding
    if (!phi::is_stable_config(heads_.size())) {
        return HydraError::CONFIG_UNSTABLE;
    }
    
    return HydraError::OK;
}

// ═══════════════════════════════════════
// REMOVE HEAD
// ═══════════════════════════════════════
HydraError HydraCore::remove_head(int head_id) {
    auto it = std::find_if(heads_.begin(), heads_.end(),
        [head_id](const std::unique_ptr<HydraHead>& h) {
            return h->id() == head_id;
        });
    
    if (it == heads_.end()) {
        return HydraError::HEAD_NOT_FOUND;
    }
    
    heads_.erase(it);
    return HydraError::OK;
}

// ═══════════════════════════════════════
// REGENERATE HEAD
// ═══════════════════════════════════════
HydraError HydraCore::regenerate_head(int head_id) {
    for (auto& head : heads_) {
        if (head->id() == head_id) {
            return head->regenerate();
        }
    }
    return HydraError::HEAD_NOT_FOUND;
}

// ═══════════════════════════════════════
// SIGN TOKEN
// ═══════════════════════════════════════
HydraError HydraCore::sign(const std::string& payload, std::string& token_out) {
    if (heads_.empty()) {
        return HydraError::CONFIG_INVALID_HEADS;
    }
    
    token_out = token_handler_.serialize(heads_);
    token_count_++;
    
    return HydraError::OK;
}

// ═══════════════════════════════════════
// VERIFY TOKEN
// ═══════════════════════════════════════
HydraError HydraCore::verify(const std::string& token,
                              ConsensusResult& result_out) {
    if (heads_.empty()) {
        return HydraError::CONFIG_INVALID_HEADS;
    }
    
    // Deserialize
    TokenConfig config;
    std::vector<TokenSignature> sigs;
    
    if (!token_handler_.deserialize(token, sigs, config)) {
        return HydraError::TOKEN_PARSE_FAILED;
    }
    
    // Extract message (header.payload)
    size_t dot2 = token.find('.', token.find('.') + 1);
    std::string message = token.substr(0, dot2);
    std::vector<uint8_t> msg_bytes(message.begin(), message.end());
    
    // Run consensus
    result_out = run_consensus(msg_bytes, sigs);
    verify_count_++;
    
    return result_out.passed ? HydraError::OK : HydraError::CONSENSUS_NOT_MET;
}

// ═══════════════════════════════════════
// RUN φ-WEIGHTED CONSENSUS
// ═══════════════════════════════════════
ConsensusResult HydraCore::run_consensus(
    const std::vector<uint8_t>& message,
    const std::vector<TokenSignature>& signatures) {
    
    ConsensusResult result;
    result.total_weight = 0.0;
    result.valid_weight = 0.0;
    
    for (auto& sig : signatures) {
        // Find matching head
        auto it = std::find_if(heads_.begin(), heads_.end(),
            [&sig](const std::unique_ptr<HydraHead>& h) {
                return h->id() == sig.head_id && h->is_active();
            });
        
        if (it == heads_.end()) continue;
        
        double weight = (*it)->weight();
        result.total_weight += weight;
        
        // Verify signature against this head
        HydraError err = (*it)->verify(message, sig.signature);
        
        if (err == HydraError::OK) {
            result.valid_weight += weight;
            result.verified_heads.push_back(sig.head_id);
        } else {
            result.failed_heads.push_back(sig.head_id);
        }
    }
    
    // Calculate ratio
    if (result.total_weight > 0.0) {
        result.ratio = result.valid_weight / result.total_weight;
        result.passed = result.ratio > threshold_;
    }
    
    // Build message
    std::stringstream ss;
    ss << "Consensus: " << (result.passed ? "PASSED" : "FAILED")
       << " (ratio: " << result.ratio
       << ", threshold: " << threshold_
       << ", valid: " << result.verified_heads.size()
       << "/" << signatures.size() << " heads)";
    result.message = ss.str();
    
    return result;
}

} // namespace hydrajwt
