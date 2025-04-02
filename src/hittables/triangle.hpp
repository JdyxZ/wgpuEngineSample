#pragma once

// Headers
#include "hittable.hpp"

// Macros
constexpr bool CULLING = false;

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

    Triangle(vertex A, vertex B, vertex C, const shared_ptr<Raytracing::Material>& material, const shared_ptr<Raytracing::Matrix44>& model = nullptr);

    bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const override;
    bool has_vertex_colors() const;
    bool has_vertex_normals() const;
    shared_ptr<Raytracing::AABB> bounding_box() const override;
    double pdf_value(const point3& hit_point, const vec3& scattering_direction) const override;
    vec3 random_scattering_ray(const point3& hit_point) const override; // https://stackoverflow.com/questions/19654251/random-point-inside-triangle-inside-java

private:
    vec3 AB, AC, N;
    double area;
    shared_ptr<Raytracing::Material> material;
    shared_ptr<Raytracing::AABB> bbox;

    pair<double, double> interpolate_texture_coordinates(double u, double v, double w) const;
};

