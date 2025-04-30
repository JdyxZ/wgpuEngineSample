#pragma once

// Headers
#include "hittable.hpp"
#include "utils/scene_stats.hpp"
#include "bvh.hpp"
#include "math/aabb.hpp"

// Forward declarations
class hittable_list;

namespace Raytracing
{
    class Surface : public Hittable
    {
    public:
	    Surface();
	    Surface(const hittable_list& triangles, const shared_ptr<Material>& material);

	    bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override;
	    AABB bounding_box() const override;
	    const bvh_stats get_stats() const;
	    const int& num_triangles() const;

    private:
	    AABB bbox;
	    bvh_node triangles;
	    shared_ptr<Material> material;
	    int _num_triangles = 0;
    };
}



