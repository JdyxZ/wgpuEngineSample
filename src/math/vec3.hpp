#pragma once

// Headers
#include "../core/core.hpp"
#include "vec.hpp"

// Forward declarations
class vec4;

class vec3 : public vec
{
public:
    double x, y, z;

    // Constructors
    vec3();
    vec3(int i);
    vec3(double d);
    vec3(double x, double y, double z);
    vec3(const vec4& v);

    // Operator overloads
    vec3 operator-() const;
    double operator[](int i) const;
    double& operator[](int i);
    vec3& operator+=(double t);
    vec3& operator-=(double t);
    vec3& operator*=(double t);
    vec3& operator/=(double t);
    vec3& operator+=(const vec3& v);
    vec3& operator-=(const vec3& v);
    vec3& operator*=(const vec3& v);
    vec3& operator/=(const vec3& v);
    friend std::ostream& operator<<(std::ostream& out, const vec3& v);

    // Length-related functions
    double length() const override;
    double length_squared() const override;
    vec3& normalize() override;
    bool near_zero() const override; // Return true if the vector is close to zero in all dimensions.

    // Static random vector generation
    static vec3 random();
    static vec3 random(double min, double max);

};

inline vec3 operator+(const vec3& u, const vec3& v)
{
    return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v)
{
    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline vec3 operator*(const vec3& u, const vec3& v)
{
    return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(double t, const vec3& v)
{
    return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, double t)
{
    return t * v;
}

inline vec3 operator/(const vec3& u, const vec3& v)
{
    return vec3(u.x / v.x, u.y / v.y, u.z / v.z);
}

inline vec3 operator/(const vec3& v, double t)
{
    return (1 / t) * v;
}

// Aliases
using point3 = vec3;

// Namespace aliases
namespace Raytracing
{
    using color = vec3;
    using normal = vec3;
    using axis = vec3;
}
