// Headers
#include "core/core.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "math/pdf.hpp"
#include "ray.hpp"
#include "hittables/hittable.hpp"
#include "utils/utilities.hpp"
#include "utils/project_parsers.hpp"

// Usings
using Raytracing::color;

// ****** Material Class ****** //

bool Raytracing::Material::scatter(const Ray& incoming_ray, const hit_record& rec, scatter_record& srec) const
{
    return false;
}

color Raytracing::Material::emitted(const Ray& incoming_ray, const hit_record& rec) const
{
    return color(0, 0, 0);
}

double Raytracing::Material::scattering_pdf_value(const Ray& incoming_ray, const hit_record& rec, const Ray& scattered_ray) const
{
    return 0;
}

const MATERIAL_TYPE Raytracing::Material::get_type() const
{
    return type;
}

// ****** Lambertian Class ****** //

Raytracing::Lambertian::Lambertian(const color& albedo) : texture(make_shared<SolidColor>(albedo))
{ 
    type = LAMBERTIAN; 
}

Raytracing::Lambertian::Lambertian(shared_ptr<Raytracing::Texture> texture) : texture(texture)
{ 
    type = LAMBERTIAN; 
}

bool Raytracing::Lambertian::scatter(const Ray& incoming_ray, const hit_record& rec, scatter_record& srec) const
{
    // auto scatter_direction = rec.normal + random_unit_vector();
    srec.is_specular = false;
    srec.specular_ray = nullopt;
    srec.pdf = make_shared<cosine_hemisphere_pdf>(rec.normal);
    srec.scatter_type = REFLECT;

    // Texture coordinates
    ImageTexture* image_texture = dynamic_cast<ImageTexture*>(texture.get());

    if (image_texture)
    {
        optional<pair<double, double>> parsed_texture_uvs = parse_texture_uvs(rec.texture_coordinates, image_texture->get_uv_wrap_modes());
        srec.attenuation = texture->value(parsed_texture_uvs, rec.p);
    }
    else
    {
        srec.attenuation = texture->value(rec.texture_coordinates, rec.p);
    }

    return true;
}

double Raytracing::Lambertian::scattering_pdf_value(const Ray& incoming_ray, const hit_record& rec, const Ray& scattered_ray) const
{
    auto cosine_theta = dot(rec.normal, unit_vector(scattered_ray.direction()));
    return std::fmax(0, cosine_theta / pi);
}

// ****** Isotropic Class ****** //

Raytracing::Isotropic::Isotropic(const color& albedo) : texture(make_shared<SolidColor>(albedo))
{
    type = ISOTROPIC;
}

Raytracing::Isotropic::Isotropic(shared_ptr<Raytracing::Texture> texture) : texture(texture)
{
    type = ISOTROPIC;
}

bool Raytracing::Isotropic::scatter(const Ray& incoming_ray, const hit_record& rec, scatter_record& srec) const
{
    srec.is_specular = false;
    srec.specular_ray = nullopt;
    srec.pdf = make_shared<uniform_sphere_pdf>();
    srec.attenuation = texture->value(rec.texture_coordinates, rec.p);
    srec.scatter_type = REFLECT;
    return true;
}

double Raytracing::Isotropic::scattering_pdf_value(const Ray& incoming_ray, const hit_record& rec, const Ray& scattered_ray) const
{
    return 1 / (4 * pi);
}

// ****** Metal Class ****** //

Raytracing::Metal::Metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) { type = METAL; }

bool Raytracing::Metal::scatter(const Ray& incoming_ray, const hit_record& rec, scatter_record& srec) const
{
    // Reflect the incoming ray
    vec3 reflected = reflect(incoming_ray.direction(), rec.normal);
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

    // Create reflected ray
    auto reflected_ray = Ray(rec.p, reflected, incoming_ray.time());

    // Save data into scatter record
    srec.is_specular = true;
    srec.specular_ray = Ray(reflected_ray);
    srec.pdf = nullptr;
    srec.attenuation = albedo;
    srec.scatter_type = REFLECT;

    // Absorb the ray if it's reflected into the surface
    // return dot(reflected_ray.direction(), rec.normal) > 0;

    return true;
}

// ****** Dielectric Class ****** //

Raytracing::Dielectric::Dielectric(double refraction_index) : refraction_index(refraction_index)
{ 
    type = DIELECTRIC; 
}

bool Raytracing::Dielectric::scatter(const Ray& incoming_ray, const hit_record& rec, scatter_record& srec) const
{
    // Attenuation is always 1 (the glass surface absorbs nothing)
    auto attenuation = color(1.0, 1.0, 1.0);

    // Check refractive index order
    double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

    // Calculate cosinus and sinus of theta (angle between the ray and the normal)
    vec3 unit_direction = unit_vector(incoming_ray.direction());
    double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    // Check if there is total reflection (the ray cannot refract)
    bool cannot_refract = ri * sin_theta > 1.0;

    // Check reflectance probability
    double reflect_prob = reflectance(cos_theta, ri);

    // Check if the ray should reflect or refract
    vec3 scattering_direction;
    if (cannot_refract || reflect_prob > random_number<double>())
    {
        scattering_direction = reflect(unit_direction, rec.normal);
        srec.scatter_type = REFLECT;
    }
    else
    {
        scattering_direction = refract(unit_direction, rec.normal, cos_theta, ri);
        srec.scatter_type = REFRACT;
    }

    // Create scattered ray
    auto scattered_ray = Ray(rec.p, scattering_direction, incoming_ray.time());

    // Save data into scatter record
    srec.is_specular = true;
    srec.specular_ray = Ray(scattered_ray);
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

// ****** DiffuseLight Class ****** //

Raytracing::DiffuseLight::DiffuseLight(shared_ptr<Raytracing::Texture>& texture) : texture(texture)
{ 
    type = DIFFUSE_LIGHT; 
}

Raytracing::DiffuseLight::DiffuseLight(const color& emit) : texture(make_shared<SolidColor>(emit))
{ 
    type = DIFFUSE_LIGHT; 
}

color Raytracing::DiffuseLight::emitted(const Ray& incoming_ray, const hit_record& rec) const
{
    if (!rec.front_face)
        return color(0, 0, 0);

    return texture->value(rec.texture_coordinates, rec.p);
}
