#pragma once

// Headers
#include "hittable.hpp"
#include "utils/scene_stats.hpp"
#include "bvh.hpp"
#include "math/aabb.hpp"
#include "hittable_list.hpp"
#include "ray.hpp"

namespace Raytracing
{
    class Mesh : public Hittable
    {
    public:
	    Mesh(const string& name, const hittable_list& surfaces, const optional<Raytracing::Matrix44>& model = nullopt, bool use_bvh = true);

	    bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;
        void set_bbox();
	    const string name() const;
        void set_numbers(const hittable_list& surfaces);
	    const int num_surfaces() const;
        const int num_triangles() const;
        void set_stats(const bvh_node& surface_bvh, const hittable_list& surface_list);
        const bvh_stats get_stats() const;
        const bool is_bvh() const;

    private:
	    shared_ptr<Hittable> surfaces;
	    string _name = "error.obj";
	    int _num_surfaces = 0;
        int _num_triangles = 0;
        bool use_bvh = true;
        bvh_stats stats = bvh_stats();
    };
}



