#pragma once

// Headers
#include "hittable.hpp"
#include "math/vec3.hpp"
#include "utils/scene_stats.hpp"
#include "hittables/bvh.hpp"
#include "math/aabb.hpp"

class Box : public Hittable
{
public:
	Box(point3 p0, point3 p1, const shared_ptr<Raytracing::Material>& material, const optional<Raytracing::Matrix44>& model = nullopt);

	bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override;
	Raytracing::AABB bounding_box() const override;
    const bvh_stats get_stats() const;

private:
	vec3 p0, p1;
	shared_ptr<Raytracing::Material> material;
	bvh_node sides;
	Raytracing::AABB bbox;
};


