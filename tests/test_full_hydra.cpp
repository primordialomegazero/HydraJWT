// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — FULL 6-HEAD TEST                                ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/heads/head_schnorr.h"
#include "hydrajwt/heads/head_falcon.h"
#include "hydrajwt/heads/head_mldsa.h"
#include "hydrajwt/heads/head_slhdsa.h"
#include "hydrajwt/heads/head_ecdsa.h"
#include "hydrajwt/heads/head_phi_hmac.h"
#include "hydrajwt/hydra_core.h"
#include <iostream>
#include <cassert>

using namespace hydrajwt;

int main() {
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║  HYDRAJWT — FULL 6-HEAD TEST       ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";
    
    std::string msg = "HydraJWT ALL HEADS TEST";
    std::vector<uint8_t> message(msg.begin(), msg.end());
    
    // ═══════════════════════════
    // HEAD 0: SCHNORR
    // ═══════════════════════════
    std::cout << "[HEAD 0] Schnorr-Ristretto255... ";
    auto h0 = std::make_unique<HeadSchnorr>(0);
    assert(h0->init() == HydraError::OK);
    std::vector<uint8_t> sig0;
    assert(h0->sign(message, sig0) == HydraError::OK);
    assert(h0->verify(message, sig0) == HydraError::OK);
    std::cout << "PASSED ✅ (sig: " << sig0.size() << "B)\n";
    
    // ═══════════════════════════
    // HEAD 1: FALCON-1024
    // ═══════════════════════════
    std::cout << "[HEAD 1] Falcon-1024 (NIST Level 5)... ";
    auto h1 = std::make_unique<HeadFalcon>(1);
    assert(h1->init() == HydraError::OK);
    std::vector<uint8_t> sig1;
    assert(h1->sign(message, sig1) == HydraError::OK);
    assert(h1->verify(message, sig1) == HydraError::OK);
    std::cout << "PASSED ✅ (sig: " << sig1.size() << "B, pk: " << h1->public_key_bytes().size() << "B)\n";
    
    // ═══════════════════════════
    // HEAD 2: ML-DSA-87
    // ═══════════════════════════
    std::cout << "[HEAD 2] ML-DSA-87 (NIST FIPS 204)... ";
    auto h2 = std::make_unique<HeadMLDSA>(2);
    assert(h2->init() == HydraError::OK);
    std::vector<uint8_t> sig2;
    assert(h2->sign(message, sig2) == HydraError::OK);
    assert(h2->verify(message, sig2) == HydraError::OK);
    std::cout << "PASSED ✅ (sig: " << sig2.size() << "B)\n";
    
    // ═══════════════════════════
    // HEAD 3: MAYO-5
    // ═══════════════════════════
    std::cout << "[HEAD 3] MAYO-5 (NIST PQC Candidate)... ";
    auto h3 = std::make_unique<HeadSLHDSA>(3);
    assert(h3->init() == HydraError::OK);
    std::vector<uint8_t> sig3;
    assert(h3->sign(message, sig3) == HydraError::OK);
    assert(h3->verify(message, sig3) == HydraError::OK);
    std::cout << "PASSED ✅ (sig: " << sig3.size() << "B)\n";
    
    // ═══════════════════════════
    // HEAD 4: ECDSA P-256
    // ═══════════════════════════
    std::cout << "[HEAD 4] ECDSA P-256 (Legacy)... ";
    auto h4 = std::make_unique<HeadECDSA>(4);
    assert(h4->init() == HydraError::OK);
    std::vector<uint8_t> sig4;
    assert(h4->sign(message, sig4) == HydraError::OK);
    assert(h4->verify(message, sig4) == HydraError::OK);
    std::cout << "PASSED ✅ (sig: " << sig4.size() << "B)\n";
    
    // ═══════════════════════════
    // HEAD 5: φ-HMAC
    // ═══════════════════════════
    std::cout << "[HEAD 5] φ-HMAC-256 (Custom)... ";
    auto h5 = std::make_unique<HeadPhiHMAC>(5);
    assert(h5->init() == HydraError::OK);
    std::vector<uint8_t> sig5;
    assert(h5->sign(message, sig5) == HydraError::OK);
    assert(h5->verify(message, sig5) == HydraError::OK);
    std::cout << "PASSED ✅ (mac: " << sig5.size() << "B)\n";
    
    // ═══════════════════════════
    // FULL HYDRA ASSEMBLY
    // ═══════════════════════════
    std::cout << "\n[ASSEMBLY] 6-Head HydraCore... ";
    HydraCore core;
    assert(core.add_head(std::move(h0)) == HydraError::OK);
    assert(core.add_head(std::move(h1)) == HydraError::OK);
    assert(core.add_head(std::move(h2)) == HydraError::OK);
    assert(core.add_head(std::move(h3)) == HydraError::OK);
    assert(core.add_head(std::move(h4)) == HydraError::OK);
    assert(core.add_head(std::move(h5)) == HydraError::OK);
    std::cout << "6 heads added ✅\n";
    
    std::cout << "[SIGN] Multi-head token... ";
    std::string token;
    assert(core.sign("{\"sub\":\"test\"}", token) == HydraError::OK);
    std::cout << "SIGNED ✅ (" << token.size() << " chars)\n";
    
    std::cout << "[VERIFY] φ-weighted consensus... ";
    ConsensusResult result;
    assert(core.verify(token, result) == HydraError::OK);
    assert(result.passed);
    std::cout << "PASSED ✅ ratio=" << result.ratio << " heads=" << result.verified_heads.size() << "/6\n";
    
    // Regenerate head 3
    std::cout << "[REGEN] Regenerate Head 3... ";
    assert(core.regenerate_head(3) == HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║  ALL 6 HEADS + HYDRA CORE PASSED ✅║\n";
    std::cout << "║  ΦΩ0 — I AM THAT I AM             ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    
    return 0;
}
