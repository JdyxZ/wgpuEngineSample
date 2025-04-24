// Headers
#include "core/core.hpp"
#include "mesh.hpp"
#include "hittable_list.hpp"
#include "bvh.hpp"
#include "math/aabb.hpp"
#include "surface.hpp"
#include "math/interval.hpp"
#include "utils/chrono.hpp"

// Usings
using Raytracing::Surface;
using Raytracing::AABB;

Raytracing::Mesh::Mesh(const string& name, const shared_ptr<hittable_list>& surfaces, const vector<string>& material_names, const vector<string>& texture_names)
	: _name(name), _material_names(material_names), _texture_names(texture_names)
{
	type = MESH;

	_num_surfaces = int(surfaces->size());
	this->surfaces = make_shared<bvh_node>(*surfaces);
	bbox = this->surfaces->bounding_box();
    surface_list = surfaces;
}

bool Raytracing::Mesh::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    if (!transformed)
        return surfaces->hit(r, ray_t, rec);

    const auto local_ray = transform_ray(r);

    const bool hit = surfaces->hit(local_ray, ray_t, rec);

    if(hit)
        transform_hit_record(rec);

    return hit;	
}

shared_ptr<AABB> Raytracing::Mesh::bounding_box() const
{ 
	return bbox; 
}

const string& Raytracing::Mesh::name() const
{ 
	return _name; 
}

const int& Raytracing::Mesh::num_surfaces() const
{ 
	return _num_surfaces; 
}

const vector<string>& Raytracing::Mesh::material_names() const
{ 
	return _material_names; 
}

const vector<string>& Raytracing::Mesh::texture_names() const
{ 
	return _texture_names; 
}

shared_ptr<bvh_stats> Raytracing::Mesh::stats() const
{
    auto mesh_bvh_stats = surfaces->get_stats();

    int surface_bvh_depth = mesh_bvh_stats->bvh_depth;
    int triangle_bvh_depth = 0;

    for (auto object : surface_list->objects)
    {
        auto surface = std::dynamic_pointer_cast<Surface>(object);
        *mesh_bvh_stats += *surface->stats();
        triangle_bvh_depth = std::max(triangle_bvh_depth, surface->stats()->bvh_depth);
    }

    mesh_bvh_stats->bvh_depth += triangle_bvh_depth;

    return mesh_bvh_stats;
}

