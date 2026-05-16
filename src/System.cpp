#include "System.hpp"
#include <cmath>

std::vector<Vec3> System::accelerations_at(const std::vector<Vec3>& positions) const {
    const size_t N = bodies.size();
    std::vector<Vec3> acc(N);  // zero-initialised

    const double eps2 = epsilon * epsilon;

    // O(N^2) double loop. For each pair (i, j) with i < j, compute the
    // pairwise force once and apply it to BOTH bodies (Newton's third law:
    // equal and opposite). This halves the work versus iterating over all
    // ordered pairs.
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            // Vector from i to j
            const Vec3 dr = positions[j] - positions[i];

            // Softened squared distance
            const double r2     = dr.norm_squared() + eps2;
            const double r2_sqrt = std::sqrt(r2);
            const double inv_r3  = 1.0 / (r2 * r2_sqrt);  // 1 / (r^2 + eps^2)^(3/2)

            // Acceleration magnitude factor per unit mass:
            //   a_i = +G * m_j * dr / r^3   (i pulled toward j)
            //   a_j = -G * m_i * dr / r^3   (j pulled toward i)
            // Same `dr` and `inv_r3`, just different mass and sign.
            const Vec3 a_ij_unit_mass = G * dr * inv_r3;

            acc[i] += a_ij_unit_mass * bodies[j].mass;
            acc[j] -= a_ij_unit_mass * bodies[i].mass;
        }
    }

    return acc;
}

double System::total_energy() const {
    // Kinetic energy: sum of (1/2) m v^2
    double KE = 0.0;
    for (const auto& b : bodies) {
        KE += 0.5 * b.mass * b.velocity.norm_squared();
    }

    // Potential energy: sum over pairs of -G m_i m_j / r
    // (negative because gravity is attractive — bound systems have negative PE)
    double PE = 0.0;
    const size_t N = bodies.size();
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            const Vec3 dr = bodies[j].position - bodies[i].position;
            const double r = std::sqrt(dr.norm_squared() + epsilon * epsilon);
            PE -= G * bodies[i].mass * bodies[j].mass / r;
        }
    }

    return KE + PE;
}
