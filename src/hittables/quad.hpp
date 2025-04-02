#pragma once

// Headers
#include "hittable.hpp"

class Quad : public Hittable 
{
public:
    Quad(point3 Q, vec3 u, vec3 v, const shared_ptr<Raytracing::Material>& material, const shared_ptr<Raytracing::Matrix44>& model = nullptr, bool pdf = false);

    void set_bounding_box();
    shared_ptr<Raytracing::AABB> bounding_box() const override;
    bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const override;
    double pdf_value(const point3& hit_point, const vec3& scattering_direction) const override;
    vec3 random_scattering_ray(const point3& hit_point) const override;
    shared_ptr<Raytracing::Material> get_material();

private:
    point3 Q;
    double D;
    vec3 u, v, w, normal;
    double area;
    shared_ptr<Raytracing::Material> material;
    shared_ptr<Raytracing::AABB> bbox;
};

