#pragma once
#include <string>
#include "Vec3.hpp"

struct Body {
    std::string name;
    double      mass;      // in solar masses (M_sun)
    Vec3        position;  // in AU
    Vec3        velocity;  // in AU/year
};
