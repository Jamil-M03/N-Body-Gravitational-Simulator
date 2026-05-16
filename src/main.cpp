#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Config.hpp"
#include "Integrator.hpp"
#include "System.hpp"

static void usage(const char* argv0) {
    std::fprintf(stderr,
        "Usage: %s <scenario.json> <output_dir>\n"
        "  Writes <output_dir>/trajectory.csv and <output_dir>/energy.csv\n",
        argv0);
}

int main(int argc, char** argv) {
    if (argc != 3) { usage(argv[0]); return 1; }
    const std::string scenario_path = argv[1];
    const std::string out_dir       = argv[2];

    Scenario scn;
    try {
        scn = load_scenario(scenario_path);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Config error: %s\n", e.what());
        return 1;
    }

    System&         sys = scn.system;
    const SimParams P   = scn.params;

    std::printf("Scenario:    %s\n", P.name.c_str());
    std::printf("Bodies:      %zu\n", sys.bodies.size());
    std::printf("G:           %.6f (units of AU^3 / M_sun / yr^2)\n", sys.G);
    std::printf("dt:          %g yr\n", P.dt);
    std::printf("total_time:  %g yr\n", P.total_time);
    std::printf("output_dt:   %g yr\n", P.output_dt);
    std::puts("");

    // Open output files
    std::ofstream traj(out_dir + "/trajectory.csv");
    std::ofstream eng (out_dir + "/energy.csv");
    if (!traj || !eng) {
        std::fprintf(stderr, "Cannot write to output dir: %s\n", out_dir.c_str());
        return 1;
    }
    traj << "t,body,x,y,z,vx,vy,vz\n";
    eng  << "t,energy,relative_drift\n";

    // Lambda to dump current state to CSV.
    auto write_frame = [&](double t) {
        for (const auto& b : sys.bodies) {
            traj << t << ',' << b.name << ','
                 << b.position.x << ',' << b.position.y << ',' << b.position.z << ','
                 << b.velocity.x << ',' << b.velocity.y << ',' << b.velocity.z << '\n';
        }
    };

    const double E0 = sys.total_energy();
    write_frame(0.0);
    eng << 0.0 << ',' << E0 << ',' << 0.0 << '\n';

    // Main loop.
    //
    // We accumulate simulation time and compare it against the next scheduled
    // output time. This is more robust than `if (step % N == 0)` because it
    // decouples output cadence from integration step count -- you can change
    // dt without changing how often frames get written.
    double t              = 0.0;
    double next_output_t  = P.output_dt;
    long   steps_taken    = 0;
    long   frames_written = 1;  // we already wrote t=0

    while (t < P.total_time) {
        // Don't overshoot the final time: clamp the last step if needed.
        double step_dt = P.dt;
        if (t + step_dt > P.total_time) step_dt = P.total_time - t;

        rk4_step(sys, step_dt);
        t += step_dt;
        ++steps_taken;

        // Write a frame if we've crossed an output threshold.
        // (A floating-point >= comparison with tolerance avoids missing
        // frames due to round-off when dt evenly divides output_dt.)
        if (t + 1e-12 >= next_output_t) {
            write_frame(t);
            const double E = sys.total_energy();
            eng << t << ',' << E << ',' << (E - E0) / std::abs(E0) << '\n';
            next_output_t += P.output_dt;
            ++frames_written;
        }
    }

    const double E1 = sys.total_energy();
    std::printf("Done.\n");
    std::printf("Steps taken:    %ld\n", steps_taken);
    std::printf("Frames written: %ld\n", frames_written);
    std::printf("Energy drift:   %.3e (%.6f%%)\n",
                std::abs((E1 - E0) / E0),
                std::abs((E1 - E0) / E0) * 100.0);
    return 0;
}
