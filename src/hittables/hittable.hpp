#pragma once

// Headers
#include "core/core.hpp"
#include "math/vec3.hpp"
#include "math/matrix.hpp"

// Forward declarations
class Ray;
class Interval;

// Namespace forward declarations
namespace Raytracing
{
    class Material;
    class AABB;
    struct Transform;
}

enum PRIMITIVE
{
	SPHERE,
	TRIANGLE,
    QUAD,
    BOX,
    CONSTANT_MEDIUM,
    MESH,
    SURFACE,
    BVH_NODE,
	NOT_SPECIFIED
};

struct barycentric_coordinates
{
	double u, v, w;
};

class hit_record 
{
public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    shared_ptr<Raytracing::Material> material;
    pair<double, double> texture_coordinates;
    PRIMITIVE type = NOT_SPECIFIED;
    vector<unsigned long long> elapsed_nanoseconds;

    virtual ~hit_record() = default; 

    void determine_normal_direction(const vec3& ray_direction, const vec3& outward_normal);     // Sets the hit record normal vector direction.
};

class sphere_hit_record : public hit_record
{
public:
    sphere_hit_record();
};

class triangle_hit_record : public hit_record
{
public:
	optional<barycentric_coordinates> bc;

    triangle_hit_record();
};

class quad_hit_record : public hit_record
{
public:
    quad_hit_record();
};

class Hittable
{
public:
    Hittable();
    virtual ~Hittable() = default;

    virtual bool hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const = 0;
    virtual shared_ptr<Raytracing::AABB> bounding_box() const = 0;
    virtual double pdf_value(const point3& hit_point, const vec3& scattering_direction) const;
    virtual vec3 random_scattering_ray(const point3& hit_point) const;
    const PRIMITIVE get_type() const;
    const bool has_pdf() const;

    void translate(const vec3& translation);
    void rotate(const vec3& axis, const double& angle);
    void scale(const vec3& scaling);

    shared_ptr<Raytracing::Matrix44> get_model() const;
    void set_model(const shared_ptr<Raytracing::Matrix44>& model);

protected:
    PRIMITIVE type = NOT_SPECIFIED;
    shared_ptr<Raytracing::Transform> transform = make_shared<Raytracing::Transform>();
    shared_ptr<Raytracing::Matrix44> model = make_shared<Raytracing::Matrix44>(Raytracing::Matrix::identity(4));
    shared_ptr<Raytracing::Matrix44> inverse_model = make_shared<Raytracing::Matrix44>(Raytracing::Matrix::identity(4));
    bool transformed = false;
    bool pdf = false;

    const shared_ptr<Ray> transform_ray(const shared_ptr<Ray>& r) const;
    void transform_hit_record(shared_ptr<hit_record>& rec) const;
};


