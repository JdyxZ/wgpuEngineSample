#pragma once

// Headers
#include "hittable.hpp"
#include "math/aabb.hpp"

class Quad : public Hittable 
{
public:
    Quad(point3 Q, vec3 u, vec3 v, const shared_ptr<Raytracing::Material>& material, const optional<Raytracing::Matrix44>& model = nullopt, bool transform_ray = false, bool pdf = false);

    Raytracing::AABB bounding_box() const override;
    void set_bbox(const optional<Raytracing::Matrix44>& model);
    bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override;
    double pdf_value(const point3& hit_point, const vec3& scattering_direction) const override;
    vec3 random_scattering_ray(const point3& hit_point) const override;
    shared_ptr<Raytracing::Material> get_material();

private:
    point3 Q;
    double D;
    vec3 u, v, w, normal;
    double area;
    shared_ptr<Raytracing::Material> material;
    Raytracing::AABB bbox;
};

