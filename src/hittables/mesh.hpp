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
	    Mesh(const string& name, const hittable_list& surfaces, const optional<Raytracing::Matrix44>& model = nullopt);

	    bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override;
	    Raytracing::AABB bounding_box() const override;
	    const string& name() const;
	    const int& num_surfaces() const;
        const bvh_stats get_stats() const;

    private:
	    Raytracing::AABB bbox;
	    bvh_node surfaces;
        hittable_list surface_list;
	    string _name = "error.obj";
	    int _num_surfaces = 0;
    };
}



