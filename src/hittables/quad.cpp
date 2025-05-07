// Headers
#include "core/core.hpp" 
#include "quad.hpp"
#include "ray.hpp"
#include "utils/utilities.hpp"
#include "math/matrix.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Material;
using Raytracing::Matrix44;
using Raytracing::infinity;

Quad::Quad(point3 Q, vec3 u, vec3 v, const shared_ptr<Material>& material, const optional<Matrix44>& model, bool transform_ray, bool pdf) : Q(Q), u(u), v(v), material(material)
{
    type = QUAD;
    this->pdf = pdf;

    auto n = cross(u, v);
    normal = unit_vector(n);
    D = dot(normal, Q);
    w = n / dot(n, n);
    area = n.length();

    if (transform_ray)
        set_model(model);

    set_bbox(model);
}

AABB Quad::bounding_box() const
{ 
    return bbox; 
}

void Quad::set_bbox(const optional<Raytracing::Matrix44>& model)
{
    vec3 q0, q1, q2, q3;

    if (model.has_value())
    {
        q0 = model.value() * vec4(Q, 1.0);
        q1 = model.value() * vec4(Q + u, 1.0);
        q2 = model.value() * vec4(Q + v, 1.0);
        q3 = model.value() * vec4(Q + u + v, 1.0);
    }
    else
    {
        q0 = Q;
        q1 = Q + u;
        q2 = Q + v;
        q3 = Q + u + v;
    }

    bbox = AABB(Q, Q + u, Q + v, Q + u + v);
}

bool Quad::hit(const Ray& r, Interval ray_t, hit_record& rec) const
{
    const Ray local_ray = transformed ? transform_ray(r) : r;
    auto denom = dot(normal, local_ray.direction());

    // No hit if the ray is parallel to the plane.
    if (std::fabs(denom) < kEpsilon)
        return false;

    // Calculate the ray intersection value
    auto t = (D - dot(normal, local_ray.origin())) / denom;

    // Return false if the hit point parameter t is outside the ray interval.
    if (!ray_t.contains(t))
        return false;

    // Intersection point
    auto P = local_ray.at(t);

    // Obtain intersection point's planar coordinates of the coordinate frame determined by the plane determined by Q, u and v
    vec3 phit = P - Q; // Intersection point's vector expressed in plane basis coordinates
    auto alpha = dot(w, cross(phit, v));
    auto beta = dot(w, cross(u, phit));

    // Check whether the intersection point is within the quad
    if (!Interval::unitary.contains(alpha) || !Interval::unitary.contains(beta))
        return false;

    // Hit record
    rec.t = t;
    rec.p = P;
    rec.material = material;
    rec.texture_coordinates = make_pair(alpha, beta);
    rec.determine_normal_direction(local_ray.direction(), normal);
    rec.type = type;

    if (transformed)
        transform_hit_record(rec);

    return true;
}

double Quad::pdf_value(const point3& hit_point, const vec3& scattering_direction) const
{
    hit_record rec;
    auto ray = Ray(hit_point, scattering_direction);

    if (!this->hit(ray, Interval(0.001, infinity), rec))
        return 0;

    auto distance_squared = rec.t * rec.t * scattering_direction.length_squared(); // light_hit_point - origin = t * direction
    auto cosine = fabs(dot(scattering_direction, rec.normal) / scattering_direction.length()); // scattering direction is not normalized

    return distance_squared / (cosine * area);
}

vec3 Quad::random_scattering_ray(const point3& hit_point) const
{
    auto p = Q + (random_double() * u) + (random_double() * v);
    return p - hit_point;
}

shared_ptr<Material> Quad::get_material()
{
    return material;
}
