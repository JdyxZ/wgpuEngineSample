#pragma once

// Headers
#include "hittable.hpp"

// Forward declarations
class hittable_list;
struct Chrono;
class bvh_node;

namespace Raytracing
{
    class Mesh : public Hittable
    {
    public:
	    Mesh(const string& name, const shared_ptr<hittable_list>& surfaces, const vector<string>& material_names, const vector<string>& texture_names);

	    bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const override;
	    shared_ptr<Raytracing::AABB> bounding_box() const override;
	    const shared_ptr<Chrono> bvh_chrono() const;
	    const string& name() const;
	    const int& num_triangles() const;
	    const int& num_surfaces() const;
	    const vector<string>& material_names();
	    const vector<string>& texture_names();


    private:
	    string _name = "error.obj";
	    int _num_triangles = 0;
	    int _num_surfaces = 0;
	    vector<string> _material_names;
	    vector<string> _texture_names;
	    shared_ptr<bvh_node> surfaces;
	    shared_ptr<Chrono> mesh_bvh_chrono;
	    shared_ptr<Raytracing::AABB> bbox;
    };
}



