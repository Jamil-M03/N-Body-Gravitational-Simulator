#pragma once
#include <vector>
#include "Body.hpp"

class System {
public:
    std::vector<Body> bodies;

    // G = 4*pi^2 in our units (AU^3 / (M_sun * yr^2))
    // This holds because Earth (m ~ 0, distance 1 AU) orbits the Sun (1 M_sun)
    // once per year, so v = 2*pi AU/yr and G*M = v^2 * r = 4*pi^2.
    double G       = 39.4784176043574;   // 4 * pi^2
    double epsilon = 0.0;                // Plummer softening length (AU)

    // Compute gravitational acceleration on each body, given hypothetical
    // positions. Masses are taken from `bodies` (mass doesn't change during
    // a simulation step). Returns one Vec3 per body, in the same order.
    //
    // We take positions as a parameter (rather than reading from `bodies`)
    // because RK4 needs to evaluate accelerations at four DIFFERENT
    // hypothetical states within a single timestep -- not just the current
    // state. Decoupling the position-source from the body-source makes
    // this trivial.
    std::vector<Vec3> accelerations_at(const std::vector<Vec3>& positions) const;

    // Diagnostics: total energy = KE + PE. Should be conserved (modulo
    // RK4's drift) — we'll plot this in the visualizer to validate.
    double total_energy() const;
};
