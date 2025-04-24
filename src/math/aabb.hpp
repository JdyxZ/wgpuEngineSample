#pragma once

// Headers
#include "math/vec3.hpp"
#include "interval.hpp"

// Forward declarations
class Ray;

namespace Raytracing
{
    class AABB
    {
    public:
        shared_ptr<Interval> x, y, z;

        AABB();
        AABB(const point3& a, const point3& b);
        AABB(const point3& a, const point3& b, const point3& c);
        AABB(const AABB& box0, const AABB& box1);
        AABB(const Interval& x, const Interval& y, const Interval& z);

        static const AABB& empty();
        static const AABB& universe();

        const shared_ptr<Interval>& axis_interval(int n) const;
        int longest_axis() const; // Returns the index of the longest axis of the bounding box.
        bool hit(const shared_ptr<Ray>& r, Interval ray_t) const;

    private:
        double delta = 0.0001;
        void pad_to_minimums();
    };

    // Operator overloads
    inline AABB operator+(const AABB& bbox, const vec3& offset)
    {
        return AABB(*bbox.x + offset.x, *bbox.y + offset.y, *bbox.z + offset.z);
    }

    inline AABB operator+(const vec3& offset, const AABB& bbox)
    {
        return bbox + offset;
    }

    inline AABB operator*(const AABB& bbox, const vec3& offset)
    {
        return AABB(*bbox.x * offset.x, *bbox.y * offset.y, *bbox.z * offset.z);
    }

    inline AABB operator*(const vec3& offset, const AABB& bbox)
    {
        return bbox * offset;
    }
}
