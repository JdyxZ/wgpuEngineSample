// Headers
#include "core/core.hpp"
#include "hittable_transform.hpp"
#include "ray.hpp"
#include "math/aabb.hpp"
#include "math/interval.hpp"
#include "utilities.hpp"
#include "math/quaternion.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Matrix44;
using Raytracing::infinity;

translate::translate(const shared_ptr<Hittable>& object, const vec3& offset) : object(object), offset(offset)
{
    type = object->get_type();
    bbox = compute_translated_bbox();
}

bool translate::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    // Move the ray backwards by the offset
    auto translated_ray = make_shared<Ray>(r->origin() - offset, r->direction(), r->time());

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if (!object->hit(translated_ray, ray_t, rec))
        return false;

    // Move the intersection point forwards by the offset
    rec->p += offset;

    return true;
}

shared_ptr<AABB> translate::bounding_box() const
{
    return bbox;
}

shared_ptr<AABB> translate::compute_translated_bbox()
{
    return make_shared<AABB>(*object->bounding_box() + offset);
}

rotate::rotate(const shared_ptr<Hittable>& object, vec3 axis, double angle) : object(object)
{
    // Define type
    type = object->get_type();

    // Transform degress to radians
    auto radians = degrees_to_radians(angle);

    // Normalize axis for unit quaternion
    axis.normalize();

    // Compute the rotation quaternion and its conjugate
    rotation_quat = make_shared<Quaternion>(axis, radians);
    inverse_rotation_quat = make_shared<Quaternion>(rotation_quat->conjugate());

    // Rotate the bounding box
    bbox = compute_rotated_bbox();
}

bool rotate::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    // Transform the ray from world space to object space using rotation quaternion
    vec3 rotated_origin = *rotation_quat * r->origin();
    vec3 rotated_direction = *rotation_quat * r->direction();
    auto rotated_ray = make_shared<Ray>(rotated_origin, rotated_direction, r->time());

    // Determine if the rotated ray hits the object
    if (!object->hit(rotated_ray, ray_t, rec))
        return false;

    // Transform the intersection point and normal back to world space
    rec->p = *inverse_rotation_quat * rec->p;
    rec->normal = *inverse_rotation_quat * rec->normal;

    return true;
}

shared_ptr<AABB> rotate::bounding_box() const
{
    return bbox;
}

shared_ptr<AABB> rotate::compute_rotated_bbox() const
{
    // Get the original bounding box
    shared_ptr<AABB> original_bbox = object->bounding_box();

    // Iterate and rotate the 8 corner points of the AABB and find the new min and max
    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                double x = i * original_bbox->x->max + (1 - i) * original_bbox->x->min;
                double y = j * original_bbox->y->max + (1 - j) * original_bbox->y->min;
                double z = k * original_bbox->z->max + (1 - k) * original_bbox->z->min;

                // Rotate the corner point
                vec3 rotated_corner = *rotation_quat * point3(x, y, z);

                // Update the new bounding box
                for (int c = 0; c < 3; c++)
                {
                    min[c] = std::fmin(min[c], rotated_corner[c]);
                    max[c] = std::fmax(max[c], rotated_corner[c]);
                }
            }
        }
    }

    return make_shared<AABB>(min, max);
}

scale::scale(const shared_ptr<Hittable>& object, const vec3& scale_factor) : object(object), scale_factor(scale_factor), inverse_scale(1.0 / scale_factor)
{
    type = object->get_type();
    bbox = compute_scaled_bbox();
}

bool scale::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    // Scale the ray into object space
    vec3 scaled_origin = r->origin() * inverse_scale;
    vec3 scaled_direction = r->direction() * inverse_scale;
    auto scaled_ray = make_shared<Ray>(scaled_origin, scaled_direction, r->time());

    // Check if the scaled ray hits the object
    if (!object->hit(scaled_ray, ray_t, rec))
        return false;

    // Transform the intersection point and normal back to world space
    rec->p *= scale_factor;
    rec->normal = (rec->normal * scale_factor).normalize();

    return true;
}

shared_ptr<AABB> scale::bounding_box() const
{
    return bbox;
}

shared_ptr<AABB> scale::compute_scaled_bbox() const
{
    return make_shared<AABB>(*object->bounding_box() * scale_factor);
}

transform::transform(const shared_ptr<Hittable>& object, const shared_ptr<Matrix44> model) : object(object)
{
    this->type = object->get_type();
    this->model = model;
    this->inverse_model = make_shared<Matrix44>(model->inverse());
    this->bbox = compute_transformed_bbox();
}

bool transform::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    // Scale the ray into object space
    vec3 scaled_origin = *inverse_model * vec4(r->origin(), 1.0);
    vec3 scaled_direction = *inverse_model *  vec4(r->direction(), 0.0);
    auto scaled_ray = make_shared<Ray>(scaled_origin, scaled_direction, r->time());

    // Check if the scaled ray hits the object
    if (!object->hit(scaled_ray, ray_t, rec))
        return false;

    // Transform the intersection point and normal back to world space
    rec->p = *model * vec4(rec->p, 1.0);
    rec->normal = (*model * vec4(rec->normal, 0.0)).normalize();

    return true;
}

shared_ptr<AABB> transform::bounding_box() const
{
    return bbox;
}

shared_ptr<AABB> transform::compute_transformed_bbox() const
{
    // Get the original bounding box
    shared_ptr<AABB> original_bbox = object->bounding_box();

    // Iterate and rotate the 8 corner points of the AABB and find the new min and max
    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                double x = i * original_bbox->x->max + (1 - i) * original_bbox->x->min;
                double y = j * original_bbox->y->max + (1 - j) * original_bbox->y->min;
                double z = k * original_bbox->z->max + (1 - k) * original_bbox->z->min;

                // Rotate the corner point
                vec3 rotated_corner = *model * vec4(x, y, z, 1.0);

                // Update the new bounding box
                min = min_vector(min, rotated_corner);
                max = max_vector(max, rotated_corner);
            }
        }
    }

    return make_shared<AABB>(min, max);
}
