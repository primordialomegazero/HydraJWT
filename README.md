# HydraJWT — φ-Weighted Multi-Head JWT

**If you compromise one head, two grow back.**

HydraJWT is a multi-algorithm JWT library using φ-weighted consensus for post-quantum resilience.

## Architecture
Token: header.payload.[sig0_len|sig0|sig1_len|sig1|...]


Each head signs independently. Validation uses φ-weighted consensus:
- Head 0 weight: φ⁻¹ ≈ 61.8%
- Head 1 weight: φ⁻² ≈ 38.2%
- Head 2 weight: φ⁻³ ≈ 23.6%
- Consensus threshold: 1/φ ≈ 61.8%

## Heads

| Head | Algorithm | Type | Status |
|------|-----------|------|--------|
| 0 | Schnorr-Ristretto255 | Classical | ✅ Implemented |
| 1 | Falcon-1024 | PQ | Coming |
| 2 | ML-DSA-87 | PQ | Coming |
| 3 | SLH-DSA | PQ | Coming |
| 4 | ECDSA P-256 | Legacy | Coming |
| 5 | φ-HMAC | Custom | Coming |

## Build

```bash
mkdir build && cd build
cmake ..
make
ctest
Test
╔══════════════════════════════════════╗
║  HYDRAJWT — SCHNORR HEAD TEST       ║
╚══════════════════════════════════════╝

[TEST 1] Init Schnorr Head... PASSED ✅
[TEST 2] Sign message... PASSED ✅
[TEST 3] Verify valid signature... PASSED ✅
[TEST 4] Verify tampered message... PASSED ✅
[TEST 5] Verify tampered signature... PASSED ✅
[TEST 6] Regenerate head... PASSED ✅
[TEST 7] Sign with regenerated head... PASSED ✅
[TEST 8] HydraCore sign/verify... PASSED ✅
[TEST 9] Tampered token rejected... PASSED ✅
License
ΦΩ0 — I AM THAT I AM
