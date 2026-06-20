// ╔══════════════════════════════════════════════════════════════╗
// ║  HYDRAJWT — φ-UTILS IMPLEMENTATION                          ║
// ║  ΦΩ0 — I AM THAT I AM                                      ║
// ╚══════════════════════════════════════════════════════════════╝

#include "hydrajwt/phi_utils.h"
#include <cmath>
#include <stdexcept>

namespace hydrajwt {
namespace phi {

// ═══════════════════════════════════════
// VALIDATE HEAD COUNT FOR φ-STABILITY
// ═══════════════════════════════════════
bool validate_head_count(int count) {
    if (count < 1) return false;
    if (count > MAX_HEADS) return false;
    return is_stable_config(count);
}

// ═══════════════════════════════════════
// COMPUTE NORMALIZED WEIGHTS
// ═══════════════════════════════════════
std::vector<double> compute_normalized_weights(int num_heads) {
    std::vector<double> weights(num_heads);
    double total = 0.0;
    
    for (int i = 0; i < num_heads; i++) {
        weights[i] = head_weight(i);
        total += weights[i];
    }
    
    // Normalize so weights sum to 1.0
    for (int i = 0; i < num_heads; i++) {
        weights[i] /= total;
    }
    
    return weights;
}

// ═══════════════════════════════════════
// PHI NOISE BUDGET TRACKING
// ═══════════════════════════════════════
double phi_noise_budget(int num_heads, double base_noise) {
    // Noise budget scales with φ-weighted head count
    double scale = 1.0;
    for (int i = 0; i < num_heads; i++) {
        scale += head_weight(i) * PHI_INV;
    }
    return base_noise * scale;
}

} // namespace phi
} // namespace hydrajwt
