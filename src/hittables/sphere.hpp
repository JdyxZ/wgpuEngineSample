#pragma once

// Headers
#include "hittable.hpp"
#include "math/aabb.hpp"
#include "ray.hpp"

class Sphere : public Hittable
{
public:
    Sphere(point3 static_center, const double radius, const shared_ptr<Raytracing::Material>& material, const optional<Raytracing::Matrix44>& model = nullopt, bool transform = false, bool pdf = false); // Stationary sphere
    Sphere(point3 start_center, point3 end_center, const double radius, const shared_ptr<Raytracing::Material>& material, const optional<Raytracing::Matrix44>& model = nullopt, bool transform = false); // Moving sphere

    bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;
    void set_static_bbox();
    void set_moving_bbox();
    double pdf_value(const point3& origin, const vec3& direction) const override;
    vec3 random_scattering_ray(const point3& origin) const override;

private:
    motion_vector center;
    double radius;
    shared_ptr<Raytracing::Material> material;

    static pair<double, double> get_sphere_uv(const point3& p);
    static vec3 sphere_front_face_random(double radius, double distance_squared);
};


