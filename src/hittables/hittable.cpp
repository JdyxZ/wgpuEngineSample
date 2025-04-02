// Headers
#include "core/core.hpp"
#include "hittable.hpp"
#include "utils/utilities.hpp"

void hit_record::determine_normal_direction(const vec3& ray_direction, const vec3& outward_normal)
{
    // NOTE: The parameter `outward_normal` is assumed to have unit length.
    front_face = dot(ray_direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

sphere_hit_record::sphere_hit_record()
{
    type = SPHERE;
}

triangle_hit_record::triangle_hit_record() : bc(nullopt)
{
    type = TRIANGLE;
}

quad_hit_record::quad_hit_record()
{
    type = QUAD;
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
