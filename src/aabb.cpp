// Headers
#include "core/core.hpp"
#include "aabb.hpp"
#include "ray.hpp"

Raytracing::AABB::AABB() 
{
    // The default AABB is empty, since intervals are empty by default.
} 

Raytracing::AABB::AABB(const Interval& x, const Interval& y, const Interval& z)
{
    this->x = make_shared<Interval>(x);
    this->y = make_shared<Interval>(y);
    this->z = make_shared<Interval>(z);
    pad_to_minimums();
}

Raytracing::AABB::AABB(const point3& a, const point3& b)
{
    // Treat the two points a and b as extrema for the bounding box, so we don't require a particular minimum/maximum coordinate order.
    x = make_shared<Interval>(std::min(a[0], b[0]), std::max(a[0], b[0]));
    y = make_shared<Interval>(std::min(a[1], b[1]), std::max(a[1], b[1]));
    z = make_shared<Interval>(std::min(a[2], b[2]), std::max(a[2], b[2]));

    pad_to_minimums();
}

Raytracing::AABB::AABB(const point3& a, const point3& b, const point3& c)
{
    x = make_shared<Interval>(std::min({ a[0], b[0], c[0] }), std::max({ a[0], b[0], c[0] }));
    y = make_shared<Interval>(std::min({ a[1], b[1], c[1] }), std::max({ a[1], b[1], c[1] }));
    z = make_shared<Interval>(std::min({ a[2], b[2], c[2] }), std::max({ a[2], b[2], c[2] }));

    pad_to_minimums();
}


Raytracing::AABB::AABB(const AABB& box0, const AABB& box1)
{
    // The interval contructor method automatically orders the interval values
    x = make_shared<Interval>(*box0.x, *box1.x);
    y = make_shared<Interval>(*box0.y, *box1.y);
    z = make_shared<Interval>(*box0.z, *box1.z);
}

Raytracing::AABB::AABB(const shared_ptr<AABB>& box0, const shared_ptr<AABB>& box1) : AABB(*box0, *box1) {}

const shared_ptr<Interval>& Raytracing::AABB::axis_interval(int n) const
{
    if (n == 1) return y;
    if (n == 2) return z;
    return x;
}

int Raytracing::AABB::longest_axis() const
{
    if (x->size() > y->size())
        return x->size() > z->size() ? 0 : 2;
    else
        return y->size() > z->size() ? 1 : 2;
}

bool Raytracing::AABB::hit(const shared_ptr<Ray>& r, Interval ray_t) const
{
    // ** Slab method ** //
    const point3& ray_orig = r->origin();
    const vec3& ray_dir = r->direction();

    for (int axis = 0; axis < 3; axis++)
    {
        const shared_ptr<Interval>& ax = axis_interval(axis);

        // Calculate ray intersection with current axis interval extremes
        const double adinv = 1.0 / ray_dir[axis];
        auto t0 = (ax->min - ray_orig[axis]) * adinv;
        auto t1 = (ax->max - ray_orig[axis]) * adinv;

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
    if (x->size() < delta) x = make_shared<Interval>(x->expand(delta));
    if (y->size() < delta) y = make_shared<Interval>(y->expand(delta));
    if (z->size() < delta) z = make_shared<Interval>(z->expand(delta));
}

// Static members
const Raytracing::AABB Raytracing::AABB::empty = Raytracing::AABB(Interval::empty, Interval::empty, Interval::empty);
const Raytracing::AABB Raytracing::AABB::universe = Raytracing::AABB(Interval::universe, Interval::universe, Interval::universe);
