// Headers
#include "core/core.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "pdf.hpp"
#include "ray.hpp"
#include "hittables/hittable.hpp"
#include "utils/utilities.hpp"

// Usings
using Raytracing::Texture;
using Raytracing::color;

bool Raytracing::Material::scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, scatter_record& srec) const
{
    return false;
}

color Raytracing::Material::emitted(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec) const
{
    return color(0, 0, 0);
}

double Raytracing::Material::scattering_pdf_value(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, const shared_ptr<Ray>& scattered_ray) const
{
    return 0;
}

const MATERIAL_TYPE Raytracing::Material::get_type() const
{
    return type;
}

Raytracing::Lambertian::Lambertian(const color& albedo) : texture(make_shared<SolidColor>(albedo))
{ 
    type = LAMBERTIAN; 
}

Raytracing::Lambertian::Lambertian(shared_ptr<Texture> texture) : texture(texture)
{ 
    type = LAMBERTIAN; 
}

bool Raytracing::Lambertian::scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, scatter_record& srec) const
{
    // auto scatter_direction = rec->normal + random_unit_vector();
    srec.is_specular = false;
    srec.specular_ray = nullptr;
    srec.pdf = make_shared<cosine_hemisphere_pdf>(rec->normal);
    srec.attenuation = texture->value(rec->texture_coordinates, rec->p);
    srec.scatter_type = REFLECT;
    return true;
}

double Raytracing::Lambertian::scattering_pdf_value(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, const shared_ptr<Ray>& scattered_ray) const
{
    auto cos_theta = dot(rec->normal, unit_vector(scattered_ray->direction()));
    return cos_theta < 0 ? 0 : cos_theta / pi;
}

Raytracing::Metal::Metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) { type = METAL; }

bool Raytracing::Metal::scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, scatter_record& srec) const
{
    // Reflect the incoming ray
    vec3 reflected = reflect(incoming_ray->direction(), rec->normal);
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

    // Create reflected ray
    auto reflected_ray = Ray(rec->p, reflected, incoming_ray->time());

    // Save data into scatter record
    srec.is_specular = true;
    srec.specular_ray = make_shared<Ray>(reflected_ray);
    srec.pdf = nullptr;
    srec.attenuation = albedo;
    srec.scatter_type = REFLECT;

    // Absorb the ray if it's reflected into the surface
    // return dot(reflected_ray.direction(), rec->normal) > 0;

    return true;
}

Raytracing::Dielectric::Dielectric(double refraction_index) : refraction_index(refraction_index)
{ 
    type = DIELECTRIC; 
}

bool Raytracing::Dielectric::scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, scatter_record& srec) const
{
    // Attenuation is always 1 (the glass surface absorbs nothing)
    auto attenuation = color(1.0, 1.0, 1.0);

    // Check refractive index order
    double ri = rec->front_face ? (1.0 / refraction_index) : refraction_index;

    // Calculate cosinus and sinus of theta (angle between the ray and the normal)
    vec3 unit_direction = unit_vector(incoming_ray->direction());
    double cos_theta = std::fmin(dot(-unit_direction, rec->normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    // Check if there is total reflection (the ray cannot refract)
    bool cannot_refract = ri * sin_theta > 1.0;

    // Check reflectance probability
    double reflect_prob = reflectance(cos_theta, ri);

    // Check if the ray should reflect or refract
    vec3 scattering_direction;
    if (cannot_refract || reflect_prob > random_double())
    {
        scattering_direction = reflect(unit_direction, rec->normal);
        srec.scatter_type = REFLECT;
    }
    else
    {
        scattering_direction = refract(unit_direction, rec->normal, cos_theta, ri);
        srec.scatter_type = REFRACT;
    }

    // Create scattered ray
    auto scattered_ray = Ray(rec->p, scattering_direction, incoming_ray->time());

    // Save data into scatter record
    srec.is_specular = true;
    srec.specular_ray = make_shared<Ray>(scattered_ray);
    srec.pdf = nullptr;
    srec.attenuation = attenuation;

    return true;
}

double Raytracing::Dielectric::reflectance(double cosine, double refraction_index)
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

Raytracing::DiffuseLight::DiffuseLight(shared_ptr<Texture> texture) : texture(texture)
{ 
    type = DIFFUSE_LIGHT; 
}

Raytracing::DiffuseLight::DiffuseLight(const color& emit) : texture(make_shared<SolidColor>(emit))
{ 
    type = DIFFUSE_LIGHT; 
}

color Raytracing::DiffuseLight::emitted(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec) const
{
    if (!rec->front_face)
        return color(0, 0, 0);

    return texture->value(rec->texture_coordinates, rec->p);
}

Raytracing::Isotropic::Isotropic(const color& albedo) : texture(make_shared<SolidColor>(albedo))
{ 
    type = ISOTROPIC; 
}

Raytracing::Isotropic::Isotropic(shared_ptr<Texture> texture) : texture(texture)
{ 
    type = ISOTROPIC; 
}

bool Raytracing::Isotropic::scatter(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, scatter_record& srec) const
{
    srec.is_specular = false;
    srec.specular_ray = nullptr;
    srec.pdf = make_shared<uniform_sphere_pdf>();
    srec.attenuation = texture->value(rec->texture_coordinates, rec->p);
    srec.scatter_type = REFLECT;
    return true;
}

double Raytracing::Isotropic::scattering_pdf_value(const shared_ptr<Ray>& incoming_ray, const shared_ptr<hit_record>& rec, const shared_ptr<Ray>& scattered_ray) const
{
    return 1 / (4 * pi);
}
