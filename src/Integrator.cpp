#include "Integrator.hpp"
#include "System.hpp"

namespace {

// Snapshot the system's current state into a State struct.
State current_state(const System& sys) {
    State s;
    s.r.reserve(sys.bodies.size());
    s.v.reserve(sys.bodies.size());
    for (const auto& b : sys.bodies) {
        s.r.push_back(b.position);
        s.v.push_back(b.velocity);
    }
    return s;
}

// Compute the time derivative of the state, i.e. f(y).
//   d/dt position = velocity
//   d/dt velocity = acceleration(positions)
// Returned State has the same layout as the input, but slots shift in
// meaning by one derivative (positions -> velocities,
// velocities -> accelerations).
State derivative(const System& sys, const State& s) {
    State d;
    d.r = s.v;                          // dr/dt = v
    d.v = sys.accelerations_at(s.r);    // dv/dt = a(r)
    return d;
}

// Return y + h*k, component-wise. Used to compute the intermediate
// states that RK4 samples derivatives at.
State scale_add(const State& y, double h, const State& k) {
    State out;
    out.r.resize(y.r.size());
    out.v.resize(y.v.size());
    for (size_t i = 0; i < y.r.size(); ++i) {
        out.r[i] = y.r[i] + h * k.r[i];
        out.v[i] = y.v[i] + h * k.v[i];
    }
    return out;
}

}  // namespace

void rk4_step(System& sys, double dt) {
    const State y = current_state(sys);

    // The four RK4 slope samples.
    // k1: slope at the start.
    // k2: slope at midpoint, projected there with k1.
    // k3: slope at midpoint, projected there with k2 (refined).
    // k4: slope at the end, projected there with k3.
    const State k1 = derivative(sys, y);
    const State k2 = derivative(sys, scale_add(y, dt * 0.5, k1));
    const State k3 = derivative(sys, scale_add(y, dt * 0.5, k2));
    const State k4 = derivative(sys, scale_add(y, dt,       k3));

    // y_new = y + (dt/6) * (k1 + 2*k2 + 2*k3 + k4)
    // Midpoints get double weight (Simpson's rule).
    const double sixth = dt / 6.0;
    for (size_t i = 0; i < sys.bodies.size(); ++i) {
        sys.bodies[i].position += sixth * (k1.r[i] + 2.0*k2.r[i] + 2.0*k3.r[i] + k4.r[i]);
        sys.bodies[i].velocity += sixth * (k1.v[i] + 2.0*k2.v[i] + 2.0*k3.v[i] + k4.v[i]);
    }
}
