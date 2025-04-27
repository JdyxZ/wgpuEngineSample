// Headers
#include "core/core.hpp"
#include "triangle.hpp"
#include "ray.hpp"
#include "math/aabb.hpp"
#include "utils/utilities.hpp"
#include "math/interval.hpp"
#include "math/matrix.hpp"

// Usings
using Raytracing::AABB;
using Raytracing::Material;
using Raytracing::Matrix44;
using Raytracing::infinity;

Triangle::Triangle(vertex A, vertex B, vertex C, const shared_ptr<Material>& material, const shared_ptr<Matrix44>& model)
    : A(A), B(B), C(C), material(material)
{
    type = TRIANGLE;
    set_model(model ? model : Hittable::model);

    AB = B.position - A.position;
    AC = C.position - A.position;

    auto normal = cross(AB, AC);
    auto normal_length = normal.length();
    area = 0.5 * normal_length; // Same reasoning as in the Quad class
    N = normal / normal_length; // Normalized normal

    // if (normal.length() < kEpsilon)
        // throw std::runtime_error("Triangle vertices are colinear");

    bbox = make_shared<AABB>(A.position, B.position, C.position);
}

bool Triangle::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    // Transform ray into local object space
    const auto local_ray = transformed ? transform_ray(r) : r;
    
    // Calculate P vector and determinant
    vec3 P = cross(local_ray->direction(), AC);
    double det = dot(AB, P);

    // If the determinant is negative, the triangle is back-facing.
    // If the determinant is close to 0, the ray misses the triangle (parallel).
    if (CULLING)
    {
        if (det < kEpsilon) return false;
    }
    else
    {
        if (fabs(det) < kEpsilon) return false;
    }

    // Invert determinant
    double invDet = 1 / det;

    // Get barycentric cordinate u and check if the ray hits inside the u-edge
    vec3 T = local_ray->origin() - A.position;
    double u = dot(T, P) * invDet;
    if (u < 0 || u > 1) return false;

    // Get barycentric cordinate v and check if the ray hits inside the v-edge
    vec3 Q = cross(T, AB);
    double v = dot(local_ray->direction(), Q) * invDet;
    if (v < 0 || u + v > 1) return false;

    // Get barycentric cordinate w
    double w = 1 - u - v;

    // Get value t of the ray
    double t = dot(AC, Q) * invDet;

    // Check if the intersection is within the valid range (check if the ray hits the triangle from behind or front)
    if (!ray_t.surrounds(t)) return false;

    // Hit record
    auto tri_rec = make_shared<triangle_hit_record>();
    tri_rec->t = t;
    tri_rec->p = local_ray->at(t);
    tri_rec->material = material;
    tri_rec->texture_coordinates = interpolate_texture_coordinates(u, v, w);
    tri_rec->type = type;
    tri_rec->bc = { u, v, w };
    tri_rec->determine_normal_direction(local_ray->direction(), N);

    // Polymorphic assignment
    rec = tri_rec;

    if (transformed)
        transform_hit_record(rec);

    return true;
}

bool Triangle::has_vertex_colors() const
{
    return A.color.has_value() && B.color.has_value() && C.color.has_value();
}

bool Triangle::has_vertex_normals() const
{
    return A.normal.has_value() && B.normal.has_value() && C.normal.has_value();
}

shared_ptr<AABB> Triangle::bounding_box() const
{
    return bbox;
}

double Triangle::pdf_value(const point3& hit_point, const vec3& scattering_direction) const
{
    shared_ptr<hit_record> rec;
    auto ray = make_shared<Ray>(hit_point, scattering_direction);

    if (!this->hit(ray, Interval(0.001, infinity), rec))
        return 0;

    auto distance_squared = rec->t * rec->t * scattering_direction.length_squared(); // light_hit_point - origin = t * direction
    auto cosine = fabs(dot(scattering_direction, rec->normal) / scattering_direction.length()); // scattering direction is not normalized

    return distance_squared / (cosine * area);
}

vec3 Triangle::random_scattering_ray(const point3& hit_point) const
{
    // Generate random barycentric coordinates
    auto r1 = random_double();
    auto r2 = random_double();

    // Convert to barycentric coordinates
    auto sqrt_r1 = sqrt(r1);
    double alpha = 1 - sqrt_r1;
    double beta = r2 * sqrt_r1;
    double gamma = 1 - alpha - beta;

    // Compute the random point on the triangle
    auto p = alpha * A.position + beta * B.position + gamma * C.position;

    return p - hit_point;
}

pair<double, double> Triangle::interpolate_texture_coordinates(double u, double v, double w) const
{
    // If any vertex is missing UVs, return a default (0,0) coordinate
    if (!A.uv.has_value() || !B.uv.has_value() || !C.uv.has_value())
        return { 0.0, 0.0 };

    // Retrieve the UVs from the vertices
    auto [uA, vA] = A.uv.value();
    auto [uB, vB] = B.uv.value();
    auto [uC, vC] = C.uv.value();

    // Interpolate UV coordinates using barycentric coordinates
    double u_interp = w * uA + u * uB + v * uC;
    double v_interp = w * vA + u * vB + v * vC;

    return { u_interp, v_interp };
}
