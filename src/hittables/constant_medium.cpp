// Headers
#include "core/core.hpp"
#include "constant_medium.hpp"
#include "ray.hpp"
#include "math/aabb.hpp"
#include "math/interval.hpp"
#include "materials/material.hpp"
#include "utils/utilities.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Material;
using Raytracing::Texture;
using Raytracing::color;
using Raytracing::Isotropic;
using Raytracing::infinity;

constant_medium::constant_medium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex) 
    : boundary(boundary), neg_inv_density(-1 / density), phase_function(make_shared<Isotropic>(tex))
{
    type = CONSTANT_MEDIUM;
}

constant_medium::constant_medium(shared_ptr<Hittable> boundary, double density, const color& albedo)
    : boundary(boundary), neg_inv_density(-1 / density), phase_function(make_shared<Isotropic>(albedo))
{
    type = CONSTANT_MEDIUM;
}

bool constant_medium::hit(const Ray& r, const Interval& ray_t, hit_record& rec) const
{
    hit_record rec1, rec2;

    if (!boundary->hit(r, Interval::universe, rec1))
        return false;

    if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
        return false;

    if (rec1.t < ray_t.min) rec1.t = ray_t.min;
    if (rec2.t > ray_t.max) rec2.t = ray_t.max;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    auto ray_length = r.direction().length();
    auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    auto hit_distance = neg_inv_density * std::log(random_number<double>());

    if (hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal = vec3(1, 0, 0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.material = phase_function;
    rec.type = type;

    return true;
}
