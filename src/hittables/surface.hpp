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
	    Surface() = default;
	    Surface(const hittable_list& triangles, const shared_ptr<Material>& material, const optional<Matrix44>& model = nullopt, bool use_bvh = true);

	    bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;
        void set_bbox();
        void set_stats(const bvh_node& triangle_bvh);
	    const bvh_stats get_stats() const;
        const bool is_bvh() const;
	    const int& num_triangles() const;

    private:
	    shared_ptr<Hittable> triangles;
	    shared_ptr<Material> material;
	    int _num_triangles = 0;
        bool use_bvh = true;
        bvh_stats stats = bvh_stats();
    };
}



