// Headers 
#include "core/core.hpp"
#include "surface.hpp"
#include "hittable_list.hpp"
#include "bvh.hpp"
#include "aabb.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Material;

Raytracing::Surface::Surface() {}

Raytracing::Surface::Surface(const shared_ptr<hittable_list>& triangles, const shared_ptr<Material>& material) : material(material)
{
    type = SURFACE;
	_num_triangles = int(triangles->size());
	this->triangles = make_shared<bvh_node>(*triangles);
	bbox = this->triangles->bounding_box();
}

bool Raytracing::Surface::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    if (!transformed)
        return triangles->hit(r, ray_t, rec);

    auto local_ray = transform_ray(r);

    bool hit = triangles->hit(r, ray_t, rec);

    if (hit)
        transform_hit_record(rec);

    return hit;
}

shared_ptr<AABB> Raytracing::Surface::bounding_box() const
{
	return bbox;
}

const shared_ptr<bvh_stats> Raytracing::Surface::stats() const
{
	return triangles->get_stats();;
}

const int& Raytracing::Surface::num_triangles() const
{
	return _num_triangles;
}
