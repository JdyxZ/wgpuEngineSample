#pragma once

// Headers
#include "core/core.hpp"
#include "vec.hpp"

// Forward declarations
class vec3;

class vec4 : public vec
{
public:
    double x, y, z, w;

    // Constructors
    vec4();
    vec4(int i);
    vec4(double d);
    vec4(double x, double y, double z, double w);
    vec4(const vec3& v, double w);

    // Operator overloads
    vec4 operator-() const;
    double operator[](int i) const;
    double& operator[](int i);
    vec4& operator+=(double t);
    vec4& operator-=(double t);
    vec4& operator*=(double t);
    vec4& operator/=(double t);
    vec4& operator+=(const vec4& v);
    vec4& operator-=(const vec4& v);
    vec4& operator*=(const vec4& v);
    vec4& operator/=(const vec4& v);
    friend std::ostream& operator<<(std::ostream& out, const vec4& v);

    // Length-related functions
    double length() const override;
    double length_squared() const override;
    vec4& normalize() override;
    bool near_zero() const override; // Return true if the vector is close to zero in all dimensions.

    // Static random vector generation
    static vec4 random();
    static vec4 random(double min, double max);
};

// vec4 operators overloads
inline vec4 operator+(const vec4& u, const vec4& v)
{
    return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

inline vec4 operator-(const vec4& u, const vec4& v)
{
    return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

inline vec4 operator*(const vec4& u, const vec4& v)
{
    return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

inline vec4 operator*(double t, const vec4& v)
{
    return vec4(t * v.x, t * v.y, t * v.z, t * v.w);
}

inline vec4 operator*(const vec4& v, double t)
{
    return t * v;
}

inline vec4 operator/(const vec4& u, const vec4& v)
{
    return vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w);
}

inline vec4 operator/(const vec4& v, double t)
{
    return (1 / t) * v;
}

// Aliases
using point4 = vec4;

