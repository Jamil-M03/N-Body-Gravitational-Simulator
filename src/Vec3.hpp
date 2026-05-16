#pragma once
#include <cmath>

struct Vec3 {
    double x, y, z;

    constexpr Vec3() : x(0), y(0), z(0) {}
    constexpr Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    // Component-wise arithmetic
    constexpr Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vec3 operator*(double s) const      { return {x * s,  y * s,  z * s};  }
    constexpr Vec3 operator/(double s) const      { return {x / s,  y / s,  z / s};  }

    // In-place versions (avoid temporaries in hot loops)
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(double s)      { x *= s;   y *= s;   z *= s;   return *this; }

    // Geometric ops
    constexpr double dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
    constexpr double norm_squared() const     { return x*x + y*y + z*z; }
    double norm() const                       { return std::sqrt(norm_squared()); }
};

// Scalar-on-left multiplication: 2.0 * v works as well as v * 2.0
constexpr Vec3 operator*(double s, const Vec3& v) { return v * s; }
