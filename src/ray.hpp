#pragma once

// Headers
#include "math/vec3.hpp"

class Ray 
{
public:
    Ray();
    Ray(const point3& origin, const vec3& direction); // Constant ray
    Ray(const point3& origin, const vec3& direction, double time);  // Discrete time ray

    const point3& origin() const;
    const vec3& direction() const;
    const double time() const;
    point3 at(double t) const;

private:
    point3 orig;
    vec3 dir;
    double tm;
};

// Aliases
using motion_vector = Ray;



