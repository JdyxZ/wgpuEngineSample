#pragma once

// Headers
#include "hittable.hpp"
#include "math/vec3.hpp"
#include "scene_stats.hpp"

// Forward declaration
class bvh_node;
struct Chrono;

class Box : public Hittable
{
public:
	Box(point3 p0, point3 p1, const shared_ptr<Raytracing::Material>& material, const shared_ptr<Raytracing::Matrix44>& model = nullptr);

	bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const override;
	shared_ptr<Raytracing::AABB> bounding_box() const override;
    const shared_ptr<bvh_stats> stats() const;

private:
	vec3 p0, p1;
	shared_ptr<Raytracing::Material> material;
	shared_ptr<bvh_node> sides;
	shared_ptr<Raytracing::AABB> bbox;
};


