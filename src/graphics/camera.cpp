// Headers
#include "core/core.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "ray.hpp"
#include "math/pdf.hpp"
#include "utils/utilities.hpp"
#include "utils/image_writer.hpp"
#include "graphics/color.hpp"
#include "math/interval.hpp"
#include "materials/material.hpp"
#include "hittables/triangle.hpp"
#include "graphics/raytracing_renderer.hpp"

// External headers
#include <omp.h>

// Usings
using Raytracing::Scene;
using Raytracing::ImageWriter;
using Raytracing::color;
using Raytracing::CameraData;

Raytracing::Camera::Camera()
{
    render_chrono = Chrono();
}

void Raytracing::Camera::initialize(CameraData& data, const Scene& scene, ImageWriter& image)
{
    vertical_fov = data.vertical_fov;
    defocus_angle = data.defocus_angle;
    focus_distance = data.focus_distance;

    lookfrom = data.lookfrom;
    lookat = data.lookat;
    world_up = data.world_up;

    initialize(scene, image);
}

void Raytracing::Camera::initialize(const Scene& scene, ImageWriter& image)
{
    // Determine viewport dimensions
    auto theta = degrees_to_radians(vertical_fov);
    auto h = std::tan(theta / 2);
    viewport_height = 2 * h * focus_distance;
    viewport_width = viewport_height * image.aspect_ratio;

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    view = unit_vector(lookfrom - lookat);
    side = unit_vector(cross(world_up, view));
    up = cross(view, side);

    // Calculate the vectors across the horizontal, down the vertical and orthogonal the viewport edges respectively.
    vec3 viewport_u = viewport_width * side;
    vec3 viewport_v = viewport_height * -up;
    vec3 viewport_w = focus_distance * view;

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / static_cast<double>(image.get_width());
    pixel_delta_v = viewport_v / static_cast<double>(image.get_height());

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = lookfrom - viewport_w - viewport_u / 2.0 - viewport_v / 2.0;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius = std::tan(degrees_to_radians(defocus_angle / 2.0));
    defocus_disk_u = side * defocus_radius * focus_distance;
    defocus_disk_v = up * defocus_radius * focus_distance;

    // Calculate the square root of samples per pixel and its inverse for stratified sampling
    pixel_sample_sqrt = int(sqrt(scene.samples_per_pixel));
    pixel_sample_sqrt_inv = 1.0 / pixel_sample_sqrt;

    // Log info
    Logger::info("CAMERA", "Camera settings succesfully initialized.");
}

void Raytracing::Camera::render(Scene& scene, ImageWriter& image)
{
    // Log info
    Logger::info("CAMERA", "Rendering started.");

    // Calculate primary rays to cast
    primary_rays = image.get_width() * image.get_height() * pixel_sample_sqrt * pixel_sample_sqrt;

    // Start render chrono
    render_chrono.start();

    // Progress counter
    uint32_t progress = 0;

    // Stratified sample square
    // #pragma omp parallel for collapse(2)
    for (int pixel_row = 0; pixel_row < image.get_height(); pixel_row++)
    {
        for (int pixel_column = 0; pixel_column < image.get_width(); pixel_column++)
        {
            // Final pixel color
            color pixel_color(0, 0, 0);

            // Sample points with stratified sampling for antialisasing
            for (int sample_row = 0; sample_row < pixel_sample_sqrt; sample_row++)
            {
                for (int sample_column = 0; sample_column < pixel_sample_sqrt; sample_column++)
                {
                    // Get ray sample around pixel location
                    auto sample_ray = get_ray_sample(pixel_row, pixel_column, sample_row, sample_column);

                    // Get pixel color of the sample point that ray sample points to
                    pixel_color += ray_color(sample_ray, scene.bounce_max_depth, scene);
                }
            }

            // Avarage samples
            pixel_color /= scene.samples_per_pixel;

            // Compute color
            tuple<uint8_t, uint8_t, uint8_t, uint8_t> RGBA_color = compute_color(pixel_color);

            // Save pixel color into image buffer (row-major order)
            image.write_pixel(pixel_row, pixel_column, RGBA_color);

            // Update progress
            // #pragma omp atomic
                progress++;
        }

        // Progress info
        // if(omp_get_num_threads() == 1)
            std::clog << "\rProgress: " << std::fixed << std::setprecision(2) << 100 * (progress / float(image.get_height() * image.get_width())) << ' ' << std::flush;
        // RayTracingRenderer::render_progress = pixel_row / float(image.get_height() - 1);
    }

    // End render chrono
    render_chrono.end();

    // Progress info end line
    std::cout << std::endl;

    // Benchmark rays
    rays_casted = primary_rays + background_rays + light_rays + reflected_rays + refracted_rays + unknwon_rays;
    average_rays_per_second = rays_casted / int(render_chrono.elapsed_miliseconds());
}


const Ray Raytracing::Camera::get_ray_sample(int pixel_row, int pixel_column, int sample_row, int sample_column) const
{
    auto offset = sample_square_stratified(sample_row, sample_column, pixel_sample_sqrt_inv);

    auto pixel_sample = pixel00_loc
        + ((pixel_row + offset.y) * pixel_delta_v)
        + ((pixel_column + offset.x) * pixel_delta_u);

    auto ray_origin = (defocus_angle <= 0) ? lookfrom : defocus_disk_sample(lookfrom, defocus_disk_u, defocus_disk_v);
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();

    auto ray = Ray(ray_origin, ray_direction, ray_time);

    return ray;
}

color Raytracing::Camera::ray_color(const Ray& sample_ray, int depth, const Scene& scene)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // Intersection details
    hit_record hrec;

    // Define ray intersection interval
    Interval ray_t(scene.min_hit_distance, Raytracing::infinity);

    // Background hit  
    if (!scene.intersect(sample_ray, ray_t, hrec))
    {
        background_rays++;
        return scene.sky_blend ? sky_blend(scene, sample_ray) : scene.background;
    }

    // Hit object type
    PRIMITIVE hit_object_type = hrec.type;

    // If the ray hits an object, calculate the color of the hit point
    switch (hit_object_type)
    {
    case CONSTANT_MEDIUM:

    case TRIANGLE:

    case QUAD:

    case SPHERE:
    {
        // Intersection point computed colors
        color color_from_scatter;
        color color_from_emission = hrec.material->emitted(sample_ray, hrec);

        // Material scattering details
        scatter_record srec;

        // If the ray does not scatter, it has hit an emissive material
        if (!hrec.material->scatter(sample_ray, hrec, srec))
        {
            light_rays++;
            return color_from_emission;
        }

        // Deal with specular materials apart from the rest (PDF skip)
        if (srec.is_specular)
        {
            switch (srec.scatter_type)
            {
                case REFLECT: reflected_rays++; break; // Metal or Dielectric
                case REFRACT: refracted_rays++; break; // Dielectric
            }

            return srec.attenuation * ray_color(srec.specular_ray.value(), depth - 1, scene);
        }

        // Aux variables (to make code more understandable)
        auto hittables_with_pdf = scene.hittables_with_pdf;
        auto material_pdf = srec.pdf;
        vec3 surface_hit_point = hrec.p;

        // Create the sampling PDF
        shared_ptr<PDF> sampling_pdf;

        // Determine the sampling PDF
        if (hittables_with_pdf.empty())
        {
            // Material associated samplig PDF
            sampling_pdf = material_pdf;
        }
        else
        {
            // Generate mixture of PDFs (hittable pdf + material pdf)
            auto _hittables_pdf = make_shared<hittables_pdf>(hittables_with_pdf, surface_hit_point);
            auto _mixture_pdf = make_shared<mixture_pdf>(_hittables_pdf, material_pdf);
            sampling_pdf = _mixture_pdf;
        }

        // Generate random scatter ray using the sampling PDF
        vec3 scatter_direction = sampling_pdf->generate();
        auto scattered = Ray(surface_hit_point, scatter_direction, sample_ray.time());

        // Get the weight of the generated scatter ray sample
        auto sampling_pdf_value = sampling_pdf->value(scatter_direction);

        // Get the material's associated scattering PDF
        auto scattering_pdf_value = hrec.material->scattering_pdf_value(sample_ray, hrec, scattered);

        // Update reflecting rays count
        reflected_rays++;

        // Recursive call
        color sample_color = ray_color(scattered, depth - 1, scene);

        // Bidirectional Reflectance Distribution Function (BRDF)
        color_from_scatter = (srec.attenuation * scattering_pdf_value * sample_color) / sampling_pdf_value;

        // Combine scatter and emission colors
        return color_from_emission + color_from_scatter;
    }
    default: // Unknown hit
        unknwon_rays++;
        return scene.sky_blend ? sky_blend(scene, sample_ray) : scene.background;
    }
}

color Raytracing::Camera::sky_blend(const Scene& scene, const Ray& r) const
{
    vec3 unit_direction = unit_vector(r.direction());

    auto a = 0.5 * (unit_direction.y + 1.0);
    color start_color = scene.background_primary;
    color end_color = scene.background_secondary;

    return lerp(a, start_color, end_color);
}

optional<color> Raytracing::Camera::barycentric_color_interpolation(const hit_record& rec, Triangle* t) const
{
    if (!rec.bc.has_value() || !t->has_vertex_colors())
        return nullopt;

    // Barycentric coordinates
    barycentric_coordinates bc = rec.bc.value();
    double u = bc.u;
    double v = bc.v;
    double w = bc.w;

    // Vertex colors
    vec3 color_u = t->A.color.value();
    vec3 color_v = t->B.color.value();
    vec3 color_w = t->C.color.value();

    color c = u * color_u + v * color_v + w * color_w;

    return c;
}
