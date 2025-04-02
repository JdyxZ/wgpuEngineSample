// Headers
#include "core/core.hpp"
#include "ray.hpp"

Ray::Ray() {}

Ray::Ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

Ray::Ray(const point3& origin, const vec3& direction, double time) : orig(origin), dir(direction), tm(time) {}

const point3& Ray::origin() const
{
    return orig;
}
const vec3& Ray::direction() const
{
    return dir;
}

const double Ray::time() const
{
    return tm;
}

point3 Ray::at(double t) const
{
    return orig + t * dir;
}
