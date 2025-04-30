// Headers
#include "core/core.hpp"
#include "mesh.hpp"
#include "surface.hpp"
#include "math/interval.hpp"
#include "utils/chrono.hpp"

// Usings
using Raytracing::Surface;
using Raytracing::AABB;

Raytracing::Mesh::Mesh(const string& name, const hittable_list& surfaces, const optional<Raytracing::Matrix44>& model)
	: _name(name)
{
	type = MESH;

    this->surface_list = surfaces;
	this->surfaces = bvh_node(surfaces);
	this->_num_surfaces = int(surfaces.size());

    bbox = this->surfaces.bounding_box();

    set_model(model);
}

bool Raytracing::Mesh::hit(const Ray& r, Interval ray_t, hit_record& rec) const
{
    if (!transformed)
        return surfaces.hit(r, ray_t, rec);

    const Ray local_ray = transform_ray(r);

    const bool hit = surfaces.hit(local_ray, ray_t, rec);

    if(hit)
        transform_hit_record(rec);

    return hit;	
}

AABB Raytracing::Mesh::bounding_box() const
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

const bvh_stats Raytracing::Mesh::get_stats() const
{
    bvh_stats mesh_bvh_stats = surfaces.get_stats();

    int surface_bvh_depth = mesh_bvh_stats.bvh_depth;
    int triangle_bvh_depth = 0;

    for (auto object : surface_list.objects)
    {
        auto surface = std::dynamic_pointer_cast<Surface>(object);
        mesh_bvh_stats += surface->get_stats();
        triangle_bvh_depth = std::max(triangle_bvh_depth, surface->get_stats().bvh_depth);
    }

    mesh_bvh_stats.bvh_depth += triangle_bvh_depth;

    return mesh_bvh_stats;
}

