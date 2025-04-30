// Headers
#include "core/core.hpp"
#include "hittable.hpp"
#include "utils/utilities.hpp"
#include "ray.hpp"

// Usings
using Raytracing::Matrix44;

void hit_record::determine_normal_direction(const vec3& ray_direction, const vec3& outward_normal)
{
    // NOTE: The parameter `outward_normal` is assumed to have unit length.
    front_face = dot(ray_direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

Hittable::Hittable()
{

}

double Hittable::pdf_value(const point3& hit_point, const vec3& scattering_direction) const
{
    return 0.0;
}

vec3 Hittable::random_scattering_ray(const point3& hit_point) const
{
    return vec3(1, 0, 0);
}

const PRIMITIVE Hittable::get_type() const
{
    return type;
}

const bool Hittable::has_pdf() const
{
    return pdf;
}

void Hittable::translate(const vec3& translation)
{
    transform.set_translation(translation);
    model = transform.get_model();
    inverse_model = Matrix44(model.inverse());
    transformed = true;
}

void Hittable::rotate(const vec3& axis, const double& angle)
{
    transform.set_rotation(axis, angle);
    model = transform.get_model();
    inverse_model = Matrix44(model.inverse());
    transformed = true;
}

void Hittable::scale(const vec3& scaling)
{
    transform.set_scaling(scaling);
    model = transform.get_model();
    inverse_model = Matrix44(model.inverse());
    transformed = true;
}

Raytracing::Matrix44 Hittable::get_model() const
{
    return model;
}

void Hittable::set_model(const optional<Raytracing::Matrix44>& model)
{
    if (!model.has_value())
        return;

    this->model = model.value();
    transform.set_model(this->model);
}

const Ray Hittable::transform_ray(const Ray& r) const
{
    // Transform ray into object space
    auto transformed_origin = inverse_model * vec4(r.origin(), 1.0);
    auto transformed_direction = inverse_model * vec4(r.direction(), 0.0);
    auto transformed_ray = Ray(transformed_origin, transformed_direction, r.time());

    return transformed_ray;
}

void Hittable::transform_hit_record(hit_record& rec) const
{
    // Transform hit record results back into world space
    rec.p = model * vec4(rec.p, 1.0);
    rec.normal = (model * vec4(rec.normal, 0.0)).normalize();
}
