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
	Box(point3 p0, point3 p1, const shared_ptr<Raytracing::Material>& material, const optional<Raytracing::Matrix44>& model = nullopt, bool use_bvh = true);

	bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;
    void set_bbox();
    void set_stats(const bvh_node& sides);
    const bvh_stats get_stats() const;
    const bool is_bvh() const;

private:
	vec3 p0, p1;
	shared_ptr<Raytracing::Material> material;
	shared_ptr<Hittable> sides;
    bool use_bvh = true;
    bvh_stats stats = bvh_stats();
};


