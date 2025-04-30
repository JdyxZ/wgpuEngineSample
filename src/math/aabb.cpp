// Headers
#include "core/core.hpp"
#include "aabb.hpp"
#include "ray.hpp"

Raytracing::AABB::AABB() 
{
    // The default AABB is empty, since intervals are empty by default.
}

Raytracing::AABB::AABB(const point3& a, const point3& b)
{
    // Treat the two points a and b as extrema for the bounding box, so we don't require a particular minimum/maximum coordinate order.
    x = Interval(std::min(a.x, b.x), std::max(a.x, b.x));
    y = Interval(std::min(a.y, b.y), std::max(a.y, b.y));
    z = Interval(std::min(a.z, b.z), std::max(a.z, b.z));

    pad_to_minimums();
}

Raytracing::AABB::AABB(const point3& a, const point3& b, const point3& c)
{
    x = Interval(std::min({ a.x, b.x, c.x }), std::max({ a.x, b.x, c.x }));
    y = Interval(std::min({ a.y, b.y, c.y }), std::max({ a.y, b.y, c.y }));
    z = Interval(std::min({ a.z, b.z, c.z }), std::max({ a.z, b.z, c.z }));

    pad_to_minimums();
}

Raytracing::AABB::AABB(const AABB& box0, const AABB& box1)
{
    // The interval contructor method automatically orders the interval values
    x = Interval(box0.x, box1.x);
    y = Interval(box0.y, box1.y);
    z = Interval(box0.z, box1.z);

    pad_to_minimums();
}

Raytracing::AABB::AABB(const Interval& x, const Interval& y, const Interval& z)
{
    this->x = Interval(x);
    this->y = Interval(y);
    this->z = Interval(z);

    pad_to_minimums();
}

const Raytracing::AABB& Raytracing::AABB::empty()
{
    static const AABB instance(Interval::empty, Interval::empty, Interval::empty);
    return instance;
}

const Raytracing::AABB& Raytracing::AABB::universe()
{
    static const AABB instance(Interval::universe, Interval::universe, Interval::universe);
    return instance;
}

const Interval& Raytracing::AABB::axis_interval(int n) const
{
    if (n == 1) return y;
    if (n == 2) return z;
    return x;
}

int Raytracing::AABB::longest_axis() const
{
    if (x.size() > y.size())
        return x.size() > z.size() ? 0 : 2;
    else
        return y.size() > z.size() ? 1 : 2;
}

bool Raytracing::AABB::hit(const Ray& r, Interval ray_t) const
{
    // ** Slab method ** //
    const point3& ray_orig = r.origin();
    const vec3& ray_dir = r.direction();

    for (int axis = 0; axis < 3; axis++)
    {
        const Interval& ax = axis_interval(axis);

        // Calculate ray intersection with current axis interval extremes
        const double adinv = 1.0 / ray_dir[axis];
        auto t0 = (ax.min - ray_orig[axis]) * adinv;
        auto t1 = (ax.max - ray_orig[axis]) * adinv;

        if (t0 < t1)
        {
            if (t0 > ray_t.min) ray_t.min = t0;
            if (t1 < ray_t.max) ray_t.max = t1;
        }
        else
        {
            if (t1 > ray_t.min) ray_t.min = t1;
            if (t0 < ray_t.max) ray_t.max = t0;
        }

        if (ray_t.max <= ray_t.min)
            return false;
    }

    return true;
}

void Raytracing::AABB::pad_to_minimums()
{
    // Adjust the AABB so that no side is narrower than some delta, padding if necessary.
    if (x.size() < delta) x = Interval(x.expand(delta));
    if (y.size() < delta) y = Interval(y.expand(delta));
    if (z.size() < delta) z = Interval(z.expand(delta));
}
