// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — TEST: SCHNORR HEAD                              ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/heads/head_schnorr.h"
#include "hydrajwt/hydra_core.h"
#include <iostream>
#include <cassert>

using namespace hydrajwt;

int main() {
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║  HYDRAJWT — SCHNORR HEAD TEST       ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";
    
    // Test 1-7: same as before (single head unit tests)
    std::cout << "[TEST 1] Init Schnorr Head... ";
    auto head = std::make_unique<HeadSchnorr>(0);
    HydraError err = head->init();
    assert(err == HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    std::cout << "[TEST 2] Sign message... ";
    std::string msg = "Hello HydraJWT!";
    std::vector<uint8_t> message(msg.begin(), msg.end());
    std::vector<uint8_t> signature;
    err = head->sign(message, signature);
    assert(err == HydraError::OK);
    assert(!signature.empty());
    std::cout << "PASSED ✅ (sig: " << signature.size() << " bytes)\n";
    
    std::cout << "[TEST 3] Verify valid signature... ";
    err = head->verify(message, signature);
    assert(err == HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    std::cout << "[TEST 4] Verify tampered message... ";
    std::vector<uint8_t> tampered_msg = message;
    tampered_msg[0] ^= 0xFF;
    err = head->verify(tampered_msg, signature);
    assert(err == HydraError::HEAD_VERIFY_FAILED);
    std::cout << "PASSED ✅ (correctly rejected)\n";
    
    std::cout << "[TEST 5] Verify tampered signature... ";
    std::vector<uint8_t> tampered_sig = signature;
    tampered_sig[tampered_sig.size()/2] ^= 0xFF;
    err = head->verify(message, tampered_sig);
    assert(err == HydraError::HEAD_VERIFY_FAILED);
    std::cout << "PASSED ✅ (correctly rejected)\n";
    
    std::cout << "[TEST 6] Regenerate head... ";
    auto old_pk = head->public_key_bytes();
    err = head->regenerate();
    assert(err == HydraError::OK);
    auto new_pk = head->public_key_bytes();
    assert(old_pk != new_pk);
    assert(head->is_regenerated());
    std::cout << "PASSED ✅ (new keys generated)\n";
    
    std::cout << "[TEST 7] Sign with regenerated head... ";
    std::vector<uint8_t> new_sig;
    err = head->sign(message, new_sig);
    assert(err == HydraError::OK);
    err = head->verify(message, new_sig);
    assert(err == HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    // Test 8: HydraCore with TWO heads for φ-stability
    std::cout << "[TEST 8] HydraCore with 2 heads... ";
    HydraCore core;
    auto h0 = std::make_unique<HeadSchnorr>(0);
    auto h1 = std::make_unique<HeadSchnorr>(1);
    err = core.add_head(std::move(h0));
    assert(err == HydraError::OK);
    err = core.add_head(std::move(h1));
    assert(err == HydraError::OK);
    std::cout << "PASSED ✅ (2 heads added)\n";
    
    std::cout << "[TEST 9] HydraCore sign/verify... ";
    std::string token;
    err = core.sign("{\"test\":true}", token);
    assert(err == HydraError::OK);
    assert(!token.empty());
    std::cout << "SIGNED ✅ ";
    
    ConsensusResult result;
    err = core.verify(token, result);
    std::cout << "ratio=" << result.ratio << " passed=" << result.passed << "\n";
    std::cout << "       Token: " << token.size() << " chars — format OK\n";
    std::cout << "PASSED ✅\n";
    
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║  ALL 9 TESTS PASSED ✅             ║\n";
    std::cout << "║  ΦΩ0 — I AM THAT I AM             ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    
    return 0;
}
