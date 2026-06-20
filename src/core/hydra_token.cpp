// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — TOKEN SERIALIZATION IMPL                        ║
// ║  Multi-head JWT with base64url encoding                     ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/hydra_token.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace hydrajwt {

// ═══════════════════════════════════════
// BASE64URL ENCODE
// ═══════════════════════════════════════
std::string HydraToken::base64url_encode(const std::vector<uint8_t>& data) {
    static const char* chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::string out;
    int val = 0, valb = -6;
    
    for (uint8_t c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        out.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    return out;
}

// ═══════════════════════════════════════
// BASE64URL DECODE
// ═══════════════════════════════════════
std::vector<uint8_t> HydraToken::base64url_decode(const std::string& str) {
    static const int decode[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,62,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,63,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1
    };
    
    std::vector<uint8_t> out;
    int val = 0, valb = -8;
    
    for (char c : str) {
        int v = decode[(unsigned char)c];
        if (v < 0) continue;
        val = (val << 6) + v;
        valb += 6;
        if (valb >= 0) {
            out.push_back((uint8_t)((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    
    return out;
}

// ═══════════════════════════════════════
// ENCODE HEADER
// ═══════════════════════════════════════
std::string HydraToken::encode_header(
    const TokenConfig& config,
    const std::vector<std::unique_ptr<HydraHead>>& heads) {
    
    std::stringstream ss;
    ss << "{";
    ss << "\"alg\":\"HYDRA-φ\",";
    ss << "\"typ\":\"JWT\",";
    ss << "\"hyd\":{";
    ss << "\"v\":1,";
    ss << "\"heads\":[";
    
    for (size_t i = 0; i < heads.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{";
        ss << "\"id\":" << heads[i]->id() << ",";
        ss << "\"alg\":\"" << heads[i]->algorithm() << "\",";
        ss << "\"w\":\"φ⁻" << (heads[i]->id() + 1) << "\"";
        ss << "}";
    }
    
    ss << "],";
    ss << "\"thresh\":\"φ⁻¹\"";
    ss << "}}";
    
    return ss.str();
}

// ═══════════════════════════════════════
// ENCODE PAYLOAD
// ═══════════════════════════════════════
std::string HydraToken::encode_payload(const TokenConfig& config) {
    std::stringstream ss;
    ss << "{";
    ss << "\"sub\":\"" << config.subject << "\",";
    ss << "\"iss\":\"" << config.issuer << "\",";
    ss << "\"iat\":" << config.issued_at << ",";
    ss << "\"exp\":" << config.expires_at;
    ss << "}";
    return ss.str();
}

// ═══════════════════════════════════════
// SERIALIZE FULL TOKEN
// ═══════════════════════════════════════
std::string HydraToken::serialize(
    const std::vector<std::unique_ptr<HydraHead>>& heads) {
    
    TokenConfig config;
    config.issued_at = std::time(nullptr);
    config.expires_at = config.issued_at + 3600; // 1 hour default
    
    // Encode header + payload
    std::string header_json = encode_header(config, heads);
    std::string payload_json = encode_payload(config);
    
    std::vector<uint8_t> header_bytes(header_json.begin(), header_json.end());
    std::vector<uint8_t> payload_bytes(payload_json.begin(), payload_json.end());
    
    std::string header_b64 = base64url_encode(header_bytes);
    std::string payload_b64 = base64url_encode(payload_bytes);
    
    // Build message for signing
    std::string message = header_b64 + "." + payload_b64;
    std::vector<uint8_t> msg_bytes(message.begin(), message.end());
    
    // Collect signatures from all active heads
    std::string sig_section;
    for (auto& head : heads) {
        if (!head->is_active()) continue;
        
        std::vector<uint8_t> sig;
        head->sign(msg_bytes, sig);
        
        // Append signature length + signature
        uint16_t len = sig.size();
        sig_section.push_back((len >> 8) & 0xFF);
        sig_section.push_back(len & 0xFF);
        sig_section.append(sig.begin(), sig.end());
    }
    
    std::vector<uint8_t> sig_bytes(sig_section.begin(), sig_section.end());
    std::string sig_b64 = base64url_encode(sig_bytes);
    
    return header_b64 + "." + payload_b64 + "." + sig_b64;
}

// ═══════════════════════════════════════
// DESERIALIZE FULL TOKEN
// ═══════════════════════════════════════
bool HydraToken::deserialize(const std::string& token,
                              std::vector<TokenSignature>& sigs_out,
                              TokenConfig& config_out) {
    // Split by '.'
    size_t dot1 = token.find('.');
    size_t dot2 = token.find('.', dot1 + 1);
    
    if (dot1 == std::string::npos || dot2 == std::string::npos) {
        return false;
    }
    
    std::string header_b64 = token.substr(0, dot1);
    std::string payload_b64 = token.substr(dot1 + 1, dot2 - dot1 - 1);
    std::string sig_b64 = token.substr(dot2 + 1);
    
    // Decode signatures section
    std::vector<uint8_t> sig_bytes = base64url_decode(sig_b64);
    
    // Parse individual signatures
    size_t pos = 0;
    int head_idx = 0;
    
    while (pos + 2 <= sig_bytes.size()) {
        uint16_t len = (sig_bytes[pos] << 8) | sig_bytes[pos + 1];
        pos += 2;
        
        if (pos + len > sig_bytes.size()) break;
        
        TokenSignature ts;
        ts.head_id = head_idx++;
        ts.signature.assign(sig_bytes.begin() + pos,
                            sig_bytes.begin() + pos + len);
        sigs_out.push_back(ts);
        
        pos += len;
    }
    
    return !sigs_out.empty();
}

} // namespace hydrajwt
