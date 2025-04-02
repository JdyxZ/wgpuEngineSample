#pragma once

// Headers
#include "core/core.hpp"
#include "math/vec3.hpp"

// Forward declarations
class ONB;
class Hittable;

class PDF // Probability Distribution Function (PDF)
{ 
public:
    virtual ~PDF() {};

    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

class uniform_sphere_pdf : public PDF 
{
public:
    uniform_sphere_pdf();

    double value(const vec3& direction) const override;
    vec3 generate() const override;
};

class cosine_hemisphere_pdf : public PDF 
{
public:
    cosine_hemisphere_pdf(const vec3& normal); // Generate a orthonormal basis of the hit point surface normal

    double value(const vec3& direction) const override;
    vec3 generate() const override;

private:
    shared_ptr<ONB> uvw;
};

class hittable_pdf : public PDF
{
public:
    hittable_pdf(shared_ptr<Hittable> object, const point3& hit_point);

    double value(const vec3& direction) const override;
    vec3 generate() const override;

private:
    shared_ptr<Hittable> object;
    point3 hit_point;
};

class hittables_pdf : public PDF 
{
public:
    hittables_pdf(const vector<shared_ptr<Hittable>>& hittables, const point3& hit_point);

    double value(const vec3& scattering_direction) const override;
    vec3 generate() const override;

private:
    const vector<shared_ptr<Hittable>>& hittables;
    point3 hit_point;
};

class mixture_pdf : public PDF 
{
public:
    mixture_pdf(shared_ptr<PDF> p0, shared_ptr<PDF> p1);

    double value(const vec3& direction) const override;
    vec3 generate() const override;

private:
    shared_ptr<PDF> p[2];
};


