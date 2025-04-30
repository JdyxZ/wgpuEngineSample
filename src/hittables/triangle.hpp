#pragma once

// Headers
#include "hittable.hpp"
#include "math/aabb.hpp"

struct vertex 
{
	// Compulsory vertex attributes
    vec3 position;

	// Optional vertex attributes
    optional<vec3> normal, color;
    optional<pair<double, double>> uv;
};

class Triangle : public Hittable
{
public:
    vertex A, B, C;

    Triangle(vertex A, vertex B, vertex C, const shared_ptr<Raytracing::Material>& material, const optional<Raytracing::Matrix44>& model = nullopt, bool culling = false);

    bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override;
    bool has_vertex_colors() const;
    bool has_vertex_normals() const;
    Raytracing::AABB bounding_box() const override;
    double pdf_value(const point3& hit_point, const vec3& scattering_direction) const override;
    vec3 random_scattering_ray(const point3& hit_point) const override; // https://stackoverflow.com/questions/19654251/random-point-inside-triangle-inside-java

private:
    vec3 AB, AC, N;
    double area;
    shared_ptr<Raytracing::Material> material;
    Raytracing::AABB bbox;
    bool culling;

    pair<double, double> interpolate_texture_coordinates(double u, double v, double w) const;
};

