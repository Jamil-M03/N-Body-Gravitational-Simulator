#pragma once
#include <vector>
#include "Vec3.hpp"

class System;  // forward declaration; we only need the type in the .cpp

// State vector for the system: positions and velocities of every body.
//
// This same struct serves two purposes:
//   1. The actual state of the bodies (r = position, v = velocity)
//   2. A time-derivative of state (r = dr/dt = velocity,
//                                  v = dv/dt = acceleration)
// The storage shape is identical; only the interpretation of each slot
// differs. RK4 leans on this uniformity heavily.
struct State {
    std::vector<Vec3> r;  // positions OR velocities (if derivative)
    std::vector<Vec3> v;  // velocities OR accelerations (if derivative)
};

// Advance `sys` in-place by one RK4 step of size dt.
void rk4_step(System& sys, double dt);
