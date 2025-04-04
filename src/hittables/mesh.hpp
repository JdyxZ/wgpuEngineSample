#pragma once

// Headers
#include "hittable.hpp"
#include "scene_stats.hpp"

// Forward declarations
class hittable_list;
class bvh_node;

namespace Raytracing
{
    class Mesh : public Hittable
    {
    public:
	    Mesh(const string& name, const shared_ptr<hittable_list>& surfaces, const vector<string>& material_names, const vector<string>& texture_names);

	    bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const override;
	    shared_ptr<Raytracing::AABB> bounding_box() const override;
	    const string& name() const;
	    const int& num_surfaces() const;
	    const vector<string>& material_names() const;
	    const vector<string>& texture_names() const;
        shared_ptr<bvh_stats> stats() const;

    private:
	    shared_ptr<bvh_node> surfaces;
	    shared_ptr<Raytracing::AABB> bbox;
        shared_ptr<hittable_list> surface_list;
	    string _name = "error.obj";
	    int _num_surfaces = 0;
	    vector<string> _material_names;
	    vector<string> _texture_names;
    };
}



