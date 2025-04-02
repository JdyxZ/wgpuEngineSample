#pragma once

// Headers
#include "hittable.hpp"
#include "ray.hpp"

class Sphere : public Hittable
{
public:
    Sphere(point3 static_center, const double radius, const shared_ptr<Raytracing::Material>& material, const shared_ptr<Raytracing::Matrix44>& model = nullptr, bool pdf = false); // Stationary sphere
    Sphere(point3 start_center, point3 end_center, const double radius, const shared_ptr<Raytracing::Material>& material, const shared_ptr<Raytracing::Matrix44>& model = nullptr); // Moving sphere

    bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const override;
    shared_ptr<Raytracing::AABB> bounding_box() const override;
    double pdf_value(const point3& origin, const vec3& direction) const override;
    vec3 random_scattering_ray(const point3& origin) const override;

private:
    motion_vector center;
    double radius;
    shared_ptr<Raytracing::Material> material;
    shared_ptr<Raytracing::AABB> bbox;

    static pair<double, double> get_sphere_uv(const point3& p);
    static vec3 sphere_front_face_random(double radius, double distance_squared);
};


