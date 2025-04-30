#pragma once

// Headers
#include "hittable.hpp"
#include "core/core.hpp"
#include "math/vec3.hpp"

// Namespace forward declaration
namespace Raytracing
{
    class Texture;
}

class constant_medium : public Hittable 
{
public:
    constant_medium(shared_ptr<Hittable> boundary, double density, shared_ptr<Raytracing::Texture> tex);
    constant_medium(shared_ptr<Hittable> boundary, double density, const Raytracing::color& albedo);

    bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override;
    Raytracing::AABB bounding_box() const override;

private:
    shared_ptr<Hittable> boundary;
    double neg_inv_density;
    shared_ptr<Raytracing::Material> phase_function;
};


