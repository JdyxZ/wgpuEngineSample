// Headers
#include "core/core.hpp"
#include "vec4.hpp"
#include "vec3.hpp"
#include "utils/utilities.hpp"

vec4::vec4() : x(0), y(0), z(0), w(0) {}

vec4::vec4(int i) : x(static_cast<double>(i)), y(static_cast<double>(i)), z(static_cast<double>(i)), w(static_cast<double>(i)) {}

vec4::vec4(double d) : x(d), y(d), z(d), w(d) {}

vec4::vec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

vec4::vec4(const vec3& v, double w) : x(v.x), y(v.y), z(v.z), w(w) {}

vec4 vec4::operator-() const
{
    return vec4(-x, -y, -z, -w);
}

double vec4::operator[](int i) const
{
    switch (i)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        string error = Logger::error("vec4", "Invalid operator access value!!!");
        throw std::invalid_argument(error);
    }
}

double& vec4::operator[](int i)
{
    switch (i)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        string error = Logger::error("vec4", "Invalid operator access value!!!");
        throw std::invalid_argument(error);
    }
}

vec4& vec4::operator+=(double t)
{
    x += t;
    y += t;
    z += t;
    w += t;
    return *this;
}

vec4& vec4::operator-=(double t)
{
    x -= t;
    y -= t;
    z -= t;
    w -= t;
    return *this;
}

vec4& vec4::operator*=(double t)
{
    x *= t;
    y *= t;
    z *= t;
    w *= t;
    return *this;
}

vec4& vec4::operator/=(double t)
{
    return *this *= 1 / t;
}

vec4& vec4::operator+=(const vec4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

vec4& vec4::operator-=(const vec4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

vec4& vec4::operator*=(const vec4& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
}

vec4& vec4::operator/=(const vec4& v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
    w /= v.w;
    return *this;
}

std::ostream& operator<<(std::ostream& out, const vec4& v)
{
    return out << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
}

vec3 vec4::dehomogenize() const
{
    return vec3(x/w, y/w, z/w);
}

double vec4::length() const
{
    return sqrt(length_squared());
}

double vec4::length_squared() const
{
    return x * x + y * y + z * z + w * w;
}

vec4& vec4::normalize()
{
    double len = length();
    if (len > 0)
    {
        *this /= len;
    }
    return *this;
}

bool vec4::near_zero() const
{
    return (fabs(x) < kEpsilon) && (fabs(y) < kEpsilon) && (fabs(z) < kEpsilon) && (fabs(w) < kEpsilon);
}

vec4 vec4::random()
{
    return vec4(random_number<double>(), random_number<double>(), random_number<double>(), random_number<double>());
}

vec4 vec4::random(double min, double max)
{
    return vec4(random_number<double>(min, max), random_number<double>(min, max), random_number<double>(min, max), random_number<double>(min, max));
}


