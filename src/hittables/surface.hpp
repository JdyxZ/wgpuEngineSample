#pragma once

// Headers
#include "hittable.hpp"

// Forward declarations
class hittable_list;
class bvh_node;
struct Chrono;

namespace Raytracing
{
    class Surface : public Hittable
    {
    public:
	    Surface();
	    Surface(const shared_ptr<hittable_list>& triangles, const shared_ptr<Material>& material);

	    bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const override;
	    shared_ptr<AABB> bounding_box() const override;
	    const shared_ptr<Chrono> bvh_chrono() const;
	    const int& num_triangles() const;

    private:
	    shared_ptr<bvh_node> triangles;
	    shared_ptr<Material> material;
	    shared_ptr<AABB> bbox;
	    shared_ptr<Chrono> surface_bvh_chrono;
	    int _num_triangles = 0;
    };
}



