#pragma once

// Headers
#include "core/core.hpp"
#include "math/vec3.hpp"

// Forward declarations
class Ray;
class PDF;
class hit_record;

// Namespace forward declarations
namespace Raytracing
{
    class Texture;
}

enum SCATTER_TYPE
{
    REFLECT,
    REFRACT
};

enum MATERIAL_TYPE
{
    LAMBERTIAN,
    METAL,
    DIELECTRIC,
    DIFFUSE_LIGHT,
    ISOTROPIC,
    NONE
};

struct scatter_record
{
public:
    bool is_specular;
    shared_ptr<Ray> specular_ray;
    shared_ptr<PDF> pdf;
    Raytracing::color attenuation;
    SCATTER_TYPE scatter_type;
};

namespace Raytracing
{
    class Material
    {
    public:
        virtual ~Material() = default;

        virtual bool scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, shared_ptr<scatter_record>& srec) const;
        virtual color emitted(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec) const;
        virtual double scattering_pdf_value(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, const shared_ptr<Ray>& scattered_ray) const;
        const MATERIAL_TYPE get_type() const;

    protected:
        MATERIAL_TYPE type = NONE;
    };

    class Lambertian : public Material
    {
    public:
        Lambertian(const color& albedo);
        Lambertian(shared_ptr<Texture> texture);

        bool scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, shared_ptr<scatter_record>& srec) const override;
        double scattering_pdf_value(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, const shared_ptr<Ray>& scattered_ray) const override;

    private:
        shared_ptr<Texture> texture;
    };

    class Metal : public Material
    {
    public:
        Metal(const color& albedo, double fuzz);

        bool scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, shared_ptr<scatter_record>& srec) const override;

    private:
        color albedo;
        double fuzz;
    };

    class Dielectric : public Material
    {
    public:
        Dielectric(double refraction_index);

        bool scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, shared_ptr<scatter_record>& srec) const override;

    private:
        double refraction_index; // Refractive index in vacuum or air, or the ratio of the material's refractive index over the refractive index of the enclosing media

        static double reflectance(double cosine, double refraction_index);
    };

    class DiffuseLight : public Material
    {
    public:
        DiffuseLight(shared_ptr<Texture> texture);
        DiffuseLight(const color& emit);

        color emitted(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec) const override;

    private:
        shared_ptr<Texture> texture;
    };

    class Isotropic : public Material
    {
    public:
        Isotropic(const color& albedo);
        Isotropic(shared_ptr<Texture> texture);

        bool scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, shared_ptr<scatter_record>& srec) const override;
        double scattering_pdf_value(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, const shared_ptr<Ray>& scattered_ray) const override;

    private:
        shared_ptr<Texture> texture;
    };
}

