// Headers
#include "core/core.hpp"
#include "mesh.hpp"
#include "surface.hpp"
#include "math/interval.hpp"
#include "utils/chrono.hpp"

// Usings
using Raytracing::Surface;
using Raytracing::AABB;

Raytracing::Mesh::Mesh(const string& name, const hittable_list& surfaces, const optional<Raytracing::Matrix44>& model, bool use_bvh)
	: _name(name), use_bvh(use_bvh)
{
	type = MESH;

    if (use_bvh)
    {
        auto surface_bvh = bvh_node(surfaces);
        this->surfaces = make_shared<bvh_node>(surface_bvh);
        set_stats(surface_bvh, surfaces);
    }
    else
    {
        this->surfaces = make_shared<hittable_list>(surfaces);
    }

    set_numbers(surfaces);
    set_bbox();
    set_model(model);
}

bool Raytracing::Mesh::hit(const Ray& r, const Interval& ray_t, hit_record& rec) const
{
    if (!transformed)
        return surfaces->hit(r, ray_t, rec);

    const Ray local_ray = transform_ray(r);

    const bool hit = surfaces->hit(local_ray, ray_t, rec);

    if(hit)
        transform_hit_record(rec);

    return hit;	
}

void Raytracing::Mesh::set_bbox()
{
    bbox = original_bbox = surfaces->get_bbox();
}

const string Raytracing::Mesh::name() const
{ 
	return _name; 
}

void Raytracing::Mesh::set_numbers(const hittable_list& surfaces)
{
    _num_surfaces = int(surfaces.size());

    for (auto object : surfaces.objects)
    {
        auto surface = std::dynamic_pointer_cast<Surface>(object);
        _num_triangles += surface->num_triangles();
    }
}

const int Raytracing::Mesh::num_surfaces() const
{ 
	return _num_surfaces; 
}

const int Raytracing::Mesh::num_triangles() const
{
    return _num_triangles;
}

void Raytracing::Mesh::set_stats(const bvh_node& surface_bvh, const hittable_list& surface_list)
{
    bvh_stats mesh_bvh_stats = surface_bvh.get_stats();

    int triangle_bvh_depth = 0;

    for (auto object : surface_list.objects)
    {
        auto surface = std::dynamic_pointer_cast<Surface>(object);
        mesh_bvh_stats += surface->get_stats();
        triangle_bvh_depth = std::max(triangle_bvh_depth, surface->get_stats().bvh_depth);
    }

    mesh_bvh_stats.bvh_depth += triangle_bvh_depth;

    stats = mesh_bvh_stats;
}

const bvh_stats Raytracing::Mesh::get_stats() const
{
    return stats;
}

const bool Raytracing::Mesh::is_bvh() const
{
    return use_bvh;
}

