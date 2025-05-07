// Headers 
#include "core/core.hpp"
#include "surface.hpp"
#include "hittable_list.hpp"
#include "ray.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Material;

Raytracing::Surface::Surface() {}

Raytracing::Surface::Surface(const hittable_list& triangles, const shared_ptr<Material>& material, const optional<Matrix44>& model) : material(material)
{
    type = SURFACE;
	_num_triangles = int(triangles.size());
	this->triangles = bvh_node(triangles);

	bbox = this->triangles.bounding_box();

    set_model(model);
}

bool Raytracing::Surface::hit(const Ray& r, Interval ray_t, hit_record& rec) const
{
    if (!transformed)
        return triangles.hit(r, ray_t, rec);

    const Ray local_ray = transform_ray(r);

    const bool hit = triangles.hit(r, ray_t, rec);

    if (hit)
        transform_hit_record(rec);

    return hit;
}

AABB Raytracing::Surface::bounding_box() const
{
	return bbox;
}

const bvh_stats Raytracing::Surface::get_stats() const
{
	return triangles.get_stats();
}

const int& Raytracing::Surface::num_triangles() const
{
	return _num_triangles;
}
