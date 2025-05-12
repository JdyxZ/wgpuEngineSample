// Headers 
#include "core/core.hpp"
#include "surface.hpp"
#include "hittable_list.hpp"
#include "ray.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Material;

Raytracing::Surface::Surface(const hittable_list& triangles, const shared_ptr<Material>& material, const optional<Matrix44>& model, bool use_bvh) : material(material), use_bvh(use_bvh)
{
    type = SURFACE;

    if (use_bvh)
    {
        auto triangle_bvh = bvh_node(triangles);
        this->triangles = make_shared<bvh_node>(triangles);
        set_stats(triangle_bvh);
    }
    else
    {
        this->triangles = make_shared<hittable_list>(triangles);
    }

	_num_triangles = int(triangles.size());

    set_bbox();
    set_model(model);
}

bool Raytracing::Surface::hit(const Ray& r, const Interval& ray_t, hit_record& rec) const
{
    if (!transformed)
        return triangles->hit(r, ray_t, rec);

    const Ray local_ray = transform_ray(r);

    const bool hit = triangles->hit(r, ray_t, rec);

    if (hit)
        transform_hit_record(rec);

    return hit;
}

void Raytracing::Surface::set_bbox()
{
    bbox = original_bbox = triangles->get_bbox();
}

void Raytracing::Surface::set_stats(const bvh_node& triangle_bvh)
{
    stats = triangle_bvh.get_stats();
}

const bvh_stats Raytracing::Surface::get_stats() const
{
    return stats;
}

const bool Raytracing::Surface::is_bvh() const
{
    return use_bvh;
}

const int& Raytracing::Surface::num_triangles() const
{
	return _num_triangles;
}
