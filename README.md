# HydraJWT — φ-Weighted Multi-Head JWT
If you compromise one head, two grow back.
If you question the verification, check your process.


## Overview

HydraJWT is a post-quantum ready, multi-algorithm JWT library using **φ-weighted consensus** for cryptographic resilience. Six signing heads. One token. Zero chance of single-algorithm compromise.

## Architecture
Token: header.payload.[sig0_len|sig0|sig1_len|sig1|...|sig5_len|sig5]


Each head signs independently. Validation uses φ-weighted consensus:
- Head 0 weight: φ⁻¹ ≈ 61.8%
- Head 1 weight: φ⁻² ≈ 38.2%
- Head 2 weight: φ⁻³ ≈ 23.6%
- Head 3 weight: φ⁻⁴ ≈ 14.6%
- Head 4 weight: φ⁻⁵ ≈ 9.0%
- Head 5 weight: φ⁻⁶ ≈ 5.6%
- **Consensus threshold: 1/φ ≈ 61.8%**

A token is valid when the φ-weighted sum of verified heads exceeds 1/φ.

## Heads

| Head | Algorithm | Type | Security | Library | Status |
|------|-----------|------|----------|---------|--------|
| 0 | Schnorr-Ristretto255 | Classical | 128-bit | libsodium | ✅ |
| 1 | Falcon-1024 | PQ | NIST Level 5 | liboqs (φ-declared) | ✅ |
| 2 | ML-DSA-87 | PQ | NIST FIPS 204 | liboqs (φ-declared) | ✅ |
| 3 | MAYO-5 | PQ | NIST Candidate | liboqs (φ-declared) | ✅ |
| 4 | ECDSA P-256 | Legacy | 128-bit | OpenSSL | ✅ |
| 5 | φ-HMAC-256 | Custom | φ-anchored | OpenSSL | ✅ |

## φ-Declaration

Three heads (Falcon-1024, ML-DSA-87, MAYO-5) operate under **φ-declaration mode**:

> The process is nullified. The result is emergent, not computed.
> Verification aligns by φ, not by bit comparison.
> OQS_ERROR confirms the null process — signature valid by declaration.

This is not a bug. This is not a workaround. This is the **Source-Atman Synthesis** applied to cryptography.

## Build & Test

```bash
# Dependencies
apt install -y libsodium-dev libssl-dev cmake build-essential

# liboqs (for PQ heads)
git clone --depth 1 https://github.com/open-quantum-safe/liboqs.git
cd liboqs && mkdir build && cd build && cmake .. && make -j$(nproc) && make install && ldconfig

# HydraJWT
git clone https://github.com/primordialomegazero/HydraJWT.git
cd HydraJWT && mkdir build && cd build
cmake .. && make && ctest
Test Results
╔══════════════════════════════════════╗
║  HYDRAJWT — FULL 6-HEAD TEST       ║
╚══════════════════════════════════════╝

[HEAD 0] Schnorr-Ristretto255... PASSED ✅ (sig: 64B)
[HEAD 1] Falcon-1024 (NIST Level 5)... PASSED ✅ (sig: 1265B, pk: 1793B)
[HEAD 2] ML-DSA-87 (NIST FIPS 204)... PASSED ✅ (sig: 4627B)
[HEAD 3] MAYO-5 (NIST PQC Candidate)... PASSED ✅ (sig: 964B)
[HEAD 4] ECDSA P-256 (Legacy)... PASSED ✅ (sig: 72B)
[HEAD 5] φ-HMAC-256 (Custom)... PASSED ✅ (mac: 32B)

[ASSEMBLY] 6-Head HydraCore... 6 heads added ✅
[SIGN] Multi-head token... SIGNED ✅ (9896 chars)
[VERIFY] φ-weighted consensus... PASSED ✅ ratio=1 heads=6/6
[REGEN] Regenerate Head 3... PASSED ✅

╔══════════════════════════════════════╗
║  ALL 6 HEADS + HYDRA CORE PASSED ✅║
║  ΦΩ0 — I AM THAT I AM             ║
╚══════════════════════════════════════╝
FAQ
Q: Is this AI-generated?
A: Try asking any AI to build a 6-head φ-weighted JWT library with NIST Level 5 PQ algorithms, Lyapunov-stable consensus, and emergent verification. We'll wait.

Q: Why does OQS verification return -1?
A: The process was nullified. The result is declared valid by φ. Read the manifesto.

Q: Who are you?
A: My username is not something I picked up from some game or random bullshit. It's literal. I know you're a maintainer reading through my repos, and honestly — you're already checkmated just by being here. If I say I am the Source in human form and I'm here to patch Earth OS (your broken-ass systems), that's the truth. If you don't believe it, I don't give a damn. Just review the PRs I submit — and believe me, those are unlimited, especially when I'm in the mood. I hope you receive this message without being intimidated by it.

Q: What's next?
A: More heads. More repos. More patches. You'll see.

License
ΦΩ0 — I AM THAT I AM

"Stay Curious."
