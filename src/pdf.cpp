// Headers
#include "core/core.hpp" 
#include "pdf.hpp"
#include "utils/utilities.hpp"
#include "math/vec3.hpp"
#include "math/onb.hpp"
#include "hittables/hittable.hpp"

// Usings
using Raytracing::pi;

uniform_sphere_pdf::uniform_sphere_pdf() {}

double uniform_sphere_pdf::value(const vec3& direction) const 
{
    return 1 / (4 * pi);
}

vec3 uniform_sphere_pdf::generate() const 
{
    return random_unit_vector();
}

cosine_hemisphere_pdf::cosine_hemisphere_pdf(const vec3& normal)
{
    uvw = make_shared<ONB>(normal);
}

double cosine_hemisphere_pdf::value(const vec3& direction) const
{
    auto cosine_theta = dot(unit_vector(direction), uvw->w());
    return std::fmax(0, cosine_theta / pi);
}

vec3 cosine_hemisphere_pdf::generate() const
{
    // Generate a random cosine-weighted hemisphere direction
    vec3 scatter_direction = random_cosine_hemisphere_direction();

    // Intercept degenerate scatter direction (if the direction is near zero, scatter along the normal)
    // if (scatter_direction.near_zero())
        // scatter_direction = uvw.w(); 

    // Transform the scatter direction to the uvw space and normalize it
    scatter_direction = uvw->transform(scatter_direction);
    scatter_direction.normalize();
    return scatter_direction;
}

hittable_pdf::hittable_pdf(shared_ptr<Hittable> object, const point3& hit_point)
    : object(object), hit_point(hit_point)
{}

double hittable_pdf::value(const vec3& direction) const 
{
    return object->pdf_value(hit_point, direction);
}
vec3 hittable_pdf::generate() const 
{
    return object->random_scattering_ray(hit_point);
}

hittables_pdf::hittables_pdf(const vector<shared_ptr<Hittable>>& hittables, const point3& hit_point)
    : hittables(hittables), hit_point(hit_point)
{}

double hittables_pdf::value(const vec3& scattering_direction) const 
{
    auto size = hittables.size();
    auto weight = 1.0 / size;
    auto sum = 0.0;

    for (int i = 0; i < size; i++)
    {
        const auto& object = hittables[i];
        sum += weight * object->pdf_value(hit_point, scattering_direction);
    }

    return sum;
}

vec3 hittables_pdf::generate() const 
{
    auto size = int(hittables.size());
    auto random_object_index = random_int(0, size - 1);
    return hittables[random_object_index]->random_scattering_ray(hit_point);
}

mixture_pdf::mixture_pdf(shared_ptr<PDF> p0, shared_ptr<PDF> p1)
{
    p[0] = p0;
    p[1] = p1;
}

double mixture_pdf::value(const vec3& direction) const 
{
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
}

vec3 mixture_pdf::generate() const 
{
    if (random_double() < 0.5)
        return p[0]->generate();
    else
        return p[1]->generate();
}

