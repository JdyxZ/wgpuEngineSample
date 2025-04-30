// Headers 
#include "core/core.hpp"
#include "sphere.hpp"
#include "utils/utilities.hpp"
#include "math/onb.hpp"
#include "math/matrix.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Material;
using Raytracing::Matrix44;
using Raytracing::infinity;
using Raytracing::pi;

Sphere::Sphere(point3 static_center, const double radius, const shared_ptr<Material>& material, const optional<Matrix44>& model, bool pdf) : radius(std::fmax(0, radius)), material(material)
{
    type = SPHERE;
    this->pdf = pdf;
    set_model(model);

    vec3 origin = static_center;
    vec3 direction = vec3(0);

    center = motion_vector(origin, direction);

    vec3 radius_vector = vec3(radius, radius, radius);
    bbox = AABB(static_center - radius_vector, static_center + radius_vector);
}

Sphere::Sphere(point3 start_center, point3 end_center, const double radius, const shared_ptr<Material>& material, const optional<Matrix44>& model) : radius(std::fmax(0, radius)), material(material)
{
    type = SPHERE;
    set_model(model);

    vec3 origin = start_center;
    vec3 direction = end_center - start_center;

    center = motion_vector(origin, direction);

    vec3 radius_vector = vec3(radius, radius, radius);
    AABB box1(center.at(0) - radius_vector, center.at(0) + radius_vector);
    AABB box2(center.at(1) - radius_vector, center.at(1) + radius_vector);
    bbox = AABB(box1, box2);
}

bool Sphere::hit(const Ray& r, Interval ray_t, hit_record& rec) const
{
    const Ray local_ray = transformed ? transform_ray(r) : r;

    point3 current_center = center.at(local_ray.time());

    vec3 oc = current_center - local_ray.origin();
    auto a = local_ray.direction().length_squared();
    auto h = dot(local_ray.direction(), oc); // h = -b/2
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0)
        return false;

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return false;
    }

    vec3 phit = local_ray.at(root);
    vec3 outward_normal = (phit - current_center) / radius;

    // Hit record
    rec.t = root;
    rec.p = phit;
    rec.material = material;
    rec.determine_normal_direction(local_ray.direction(), outward_normal);
    rec.texture_coordinates = get_sphere_uv(outward_normal);
    rec.type = type;

    if (transformed)
        transform_hit_record(rec);

    return true;
}

AABB Sphere::bounding_box() const
{
    return bbox;
}

double Sphere::pdf_value(const point3& origin, const vec3& direction) const
{
    // This method only works for stationary spheres.

    hit_record rec;
    auto ray = Ray(origin, direction);

    if (!this->hit(ray, Interval(0.001, infinity), rec))
        return 0;

    auto dist_squared = (center.at(0) - origin).length_squared();
    auto cos_theta_max = std::sqrt(1 - radius * radius / dist_squared);
    auto solid_angle = 2 * pi * (1 - cos_theta_max);

    return  1 / solid_angle;
}

vec3 Sphere::random_scattering_ray(const point3& origin) const
{
    vec3 direction = center.at(0) - origin;
    auto distance_squared = direction.length_squared();
    ONB uvw(direction);
    return uvw.transform(sphere_front_face_random(radius, distance_squared));
}

pair<double, double> Sphere::get_sphere_uv(const point3& p)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.

    auto theta = std::acos(-p.y);
    auto phi = std::atan2(-p.z, p.x) + pi;

    return make_pair(phi / (2 * pi), theta / pi);
}

vec3 Sphere::sphere_front_face_random(double radius, double distance_squared)
{
    auto r1 = random_double();
    auto r2 = random_double();
    auto phi = 2 * pi * r1;

    auto z = 1 + r2 * (std::sqrt(1 - radius * radius / distance_squared) - 1);
    auto x = std::cos(phi) * std::sqrt(1 - z * z);
    auto y = std::sin(phi) * std::sqrt(1 - z * z);

    return vec3(x, y, z);
}
