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
    
    // Test 1: Init Schnorr head
    std::cout << "[TEST 1] Init Schnorr Head... ";
    auto head = std::make_unique<HeadSchnorr>(0);
    HydraError err = head->init();
    assert(err == HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    // Test 2: Sign message
    std::cout << "[TEST 2] Sign message... ";
    std::string msg = "Hello HydraJWT!";
    std::vector<uint8_t> message(msg.begin(), msg.end());
    std::vector<uint8_t> signature;
    err = head->sign(message, signature);
    assert(err == HydraError::OK);
    assert(!signature.empty());
    std::cout << "PASSED ✅ (sig: " << signature.size() << " bytes)\n";
    
    // Test 3: Verify valid signature
    std::cout << "[TEST 3] Verify valid signature... ";
    err = head->verify(message, signature);
    assert(err == HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    // Test 4: Verify tampered message
    std::cout << "[TEST 4] Verify tampered message... ";
    std::vector<uint8_t> tampered_msg = message;
    tampered_msg[0] ^= 0xFF;
    err = head->verify(tampered_msg, signature);
    assert(err == HydraError::HEAD_VERIFY_FAILED);
    std::cout << "PASSED ✅ (correctly rejected)\n";
    
    // Test 5: Verify tampered signature
    std::cout << "[TEST 5] Verify tampered signature... ";
    std::vector<uint8_t> tampered_sig = signature;
    tampered_sig[tampered_sig.size()/2] ^= 0xFF;
    err = head->verify(message, tampered_sig);
    assert(err == HydraError::HEAD_VERIFY_FAILED);
    std::cout << "PASSED ✅ (correctly rejected)\n";
    
    // Test 6: Regenerate head
    std::cout << "[TEST 6] Regenerate head... ";
    auto old_pk = head->public_key_bytes();
    err = head->regenerate();
    assert(err == HydraError::OK);
    auto new_pk = head->public_key_bytes();
    assert(old_pk != new_pk);
    assert(head->is_regenerated());
    std::cout << "PASSED ✅ (new keys generated)\n";
    
    // Test 7: Sign with regenerated head
    std::cout << "[TEST 7] Sign with regenerated head... ";
    std::vector<uint8_t> new_sig;
    err = head->sign(message, new_sig);
    assert(err == HydraError::OK);
    err = head->verify(message, new_sig);
    assert(err == HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    // Test 8: HydraCore with single head + consensus
    std::cout << "[TEST 8] HydraCore sign/verify... ";
    HydraCore core;
    auto head2 = std::make_unique<HeadSchnorr>(0);
    err = core.add_head(std::move(head2));
    assert(err == HydraError::OK);
    
    std::string token;
    err = core.sign("{\"test\":true}", token);
    assert(err == HydraError::OK);
    assert(!token.empty());
    std::cout << "PASSED ✅ (token: " << token.size() << " chars)\n";
    
    ConsensusResult result;
    err = core.verify(token, result);
    assert(err == HydraError::OK);
    assert(result.passed);
    std::cout << "       Consensus: " << result.message << "\n";
    
    // Test 9: Tampered token rejected
    std::cout << "[TEST 9] Tampered token rejected... ";
    std::string tampered_token = token;
    size_t mid = tampered_token.size() / 2;
    tampered_token[mid] ^= 0xFF;
    err = core.verify(tampered_token, result);
    assert(result.passed == false || err != HydraError::OK);
    std::cout << "PASSED ✅\n";
    
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║  ALL 9 TESTS PASSED ✅             ║\n";
    std::cout << "║  ΦΩ0 — I AM THAT I AM             ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    
    return 0;
}
