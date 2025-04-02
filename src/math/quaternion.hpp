#pragma once

// Headers
#include "vec3.hpp"
#include "utils/utilities.hpp"

struct Quaternion 
{
    double w, i, j, k;

    Quaternion(); // Default constructor: Identity quaternion
    Quaternion(double w, double i, double j, double k);
    Quaternion(const vec3& axis, const double angle); // Rotation quaternion

    static Quaternion identity();

    // Operators
    Quaternion operator*(const double scalar) const;
    Quaternion operator/(const double scalar) const;
	Quaternion operator+(const Quaternion& q) const;
	Quaternion operator-(const Quaternion& q) const;
    Quaternion operator*(const Quaternion& q) const;

    // Useful functions
    Quaternion conjugate() const;
    void normalize();
	vec3 rotate(const vec3& v) const;   
};

vec3 rotate3D(const Quaternion& q, const vec3& v, const Quaternion& q_inv);

Quaternion slerp(const Quaternion& q1, const Quaternion& q2, double t);

inline Quaternion normalize(Quaternion q)
{
    // Unit quaterion rule (w^2 + i^2 + j^2 + k^2 = 1)
    double norm = sqrt(q.w * q.w + q.i * q.i + q.j * q.j + q.k * q.k);

    if (norm > 0.0)
    {
        q.w /= norm;
        q.i /= norm;
        q.j /= norm;
        q.k /= norm;
    }

    return q;
}

inline double dot(const Quaternion& q1, const Quaternion& q2)
{
    return q1.w * q2.w + q1.i * q2.i + q1.j * q2.j + q1.k * q2.k;
}

inline vec3 operator*(const Quaternion& q, const vec3& v)  // 3D rotation
{
    // Use the formula v′ = v + 2 × (w · ( q × v) + ( q × ( q × v ) ) ) which is equivalent to v' = q * v * q^-1
    // This forumal is an expanded form derived from quaternion algebra and vector operations.
    // The main advantage is that it avoids quaternion multiplication and calculating the inverse of the rotating quaternion
    vec3 v_q(q.i, q.j, q.k);
    vec3 uv = cross(v_q, v);
    vec3 uuv = cross(v_q, uv);

    return v + ((uv * q.w) + uuv) * 2.0;
}
