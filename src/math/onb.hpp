#pragma once

// Headers
#include "vec3.hpp"

class ONB  // Orthonormal Basis (ONB)
{ 
public:
    ONB(const vec3& n);

    const vec3& u() const;
    const vec3& v() const;
    const vec3& w() const;
    vec3 transform(const vec3& v) const;  // Transform from basis coordinates to local space.

private:
    vec3 axis[3];
};


