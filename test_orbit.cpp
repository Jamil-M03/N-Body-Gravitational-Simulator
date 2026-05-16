// One-off sanity test — NOT part of the final build.
// Compile manually:
//   g++ -std=c++17 -O2 -Isrc test_orbit.cpp src/System.cpp src/Integrator.cpp -o test_orbit
#include <cmath>
#include <cstdio>
#include "src/System.hpp"
#include "src/Integrator.hpp"

int main() {
    System sys;
    sys.bodies.push_back({"Sun",   1.0,    {0, 0, 0}, {0, 0, 0}});
    sys.bodies.push_back({"Earth", 3.0e-6, {1, 0, 0}, {0, 2.0 * M_PI, 0}});

    const double E0 = sys.total_energy();

    double t = 0.0;
    const double dt = 0.001;   // ~8.77 hours
    const double T  = 1.0;     // 1 year
    long steps = 0;
    while (t < T) {
        rk4_step(sys, dt);
        t += dt;
        ++steps;
    }

    const Vec3 final_pos = sys.bodies[1].position;
    const Vec3 expected{1.0, 0.0, 0.0};
    const double pos_err = (final_pos - expected).norm();
    const double E1 = sys.total_energy();
    const double E_drift = std::abs((E1 - E0) / E0);

    std::printf("Steps taken:       %ld\n", steps);
    std::printf("Earth final pos:   (%.6f, %.6f, %.6f) AU\n",
                final_pos.x, final_pos.y, final_pos.z);
    std::printf("Expected position: (1.000000, 0.000000, 0.000000) AU\n");
    std::printf("Position error:    %.2e AU  (%.2e km)\n",
                pos_err, pos_err * 1.496e8);
    std::printf("Energy drift:      %.2e (%.6f%%)\n",
                E_drift, E_drift * 100.0);

    return 0;
}
