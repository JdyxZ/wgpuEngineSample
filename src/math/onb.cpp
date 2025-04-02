// Headers
#include "core/core.hpp"
#include "onb.hpp"
#include "utils/utilities.hpp"
#include "core/framework.hpp"

ONB::ONB(const vec3& n)
{
    // Assume n is a unit vector!!!
    vec3 a = (fabs(n.x) > 0.9) ? y_axis : x_axis;

    vec3 w = n;
    vec3 v = cross(w, a);
    vec3 u = cross(w, v);

    axis[0] = u;
    axis[1] = v;
    axis[2] = w;
}

const vec3& ONB::u() const
{
    return axis[0];
}

const vec3& ONB::v() const
{
    return axis[1];
}

const vec3& ONB::w() const
{
    return axis[2];
}


vec3 ONB::transform(const vec3& v) const
{
    return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
}
