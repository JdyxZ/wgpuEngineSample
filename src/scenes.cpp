// Headers
#include "core/core.hpp"
#include "scenes.hpp"
#include "scene.hpp"
#include "hittables/bvh.hpp"
#include "core/framework.hpp"
#include "graphics/camera.hpp"
#include "utils/image_writer.hpp"
#include "materials/material.hpp"
#include "materials/texture.hpp"
#include "hittables/sphere.hpp"
#include "hittables/triangle.hpp"
#include "hittables/quad.hpp"
#include "hittables/box.hpp"
#include "hittables/mesh.hpp"
#include "hittables/constant_medium.hpp"
#include "math/perlin.hpp"
#include "math/vec3.hpp"
#include "graphics/color.hpp"
#include "utils/utilities.hpp"
#include "utils/obj_loader.hpp"
#include "utils/chrono.hpp"
#include "math/transform.hpp"

// Usings
using Raytracing::Scene;
using Raytracing::Camera;
using Raytracing::ImageWriter;
using Raytracing::Material;
using Raytracing::Lambertian;
using Raytracing::Dielectric;
using Raytracing::Metal;
using Raytracing::Mesh;
using Raytracing::color;
using Raytracing::axis;

void Raytracing::book1_final_scene_creation(Scene& scene, bool blur_motion)
{
    // Materials
    auto material_ground = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
    auto material_center = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
    auto material_left = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right = make_shared<Metal>(color(0.8, 0.6, 0.2), 1.0);
    auto material1 = make_shared<Dielectric>(1.5);
    auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
    auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);

    // Triangles (define them in counter-clockwise order)
    vertex A, B, C;
    A = vertex({ point3(0.2, -0.2, -0.5), nullopt, YELLOW });
    B = vertex({ point3(0.8, -0.2, -0.5), nullopt, CYAN });
    C = vertex({ point3(0.2, 0, -0.5), nullopt, MAGENTA });
    auto triangle1 = make_shared<Triangle>(A, B, C, material_left);

    // Random sphere creation loop
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = random_number<double>();
            point3 center(a + 0.9 * random_number<double>(), 0.2, b + 0.9 * random_number<double>());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<Material> sphere_material;

                // Diffuse
                if (choose_mat < 0.8)
                {
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<Lambertian>(albedo);

                    if (blur_motion)
                    {
                        auto center2 = center + vec3(0, random_number<double>(0, .5), 0);
                        scene.add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
                    }
                    else
                    {
                        scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    }
                }
                // Metal
                else if (choose_mat < 0.95)
                {
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_number<double>(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                // Glass
                else
                {
                    sphere_material = make_shared<Dielectric>(1.5);
                    scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // Spheres
    auto sphere1 = make_shared<Sphere>(point3(0, 1, 0), 1.0, material1);
    auto sphere2 = make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2);
    auto sphere3 = make_shared<Sphere>(point3(4, 1, 0), 1.0, material3);
    auto sphere4 = make_shared<Sphere>(point3(0, -1000, 0), 1000, material_ground);

    // Add primitives to the scene
    scene.add(sphere1);
    scene.add(sphere2);
    scene.add(sphere3);
    scene.add(sphere4);
    // scene1.add(triangle1);
}

void Raytracing::book1_final_scene(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 20;
    camera.lookfrom = point3(13, 2, 3);
    camera.lookat = point3(0, 0, 0);
    camera.defocus_angle = 0.6;

    // Scene settings
    scene.name = "Book 1 Final Scene";
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 50;

    // Create scene
    book1_final_scene_creation(scene);
}

void Raytracing::bouncing_spheres(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 20;
    camera.lookfrom = point3(13, 2, 3);
    camera.lookat = point3(0, 0, 0);
    camera.defocus_angle = 0.6;

    // Scene settings
    scene.name = "Bouncing Spheres";
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 50;

    // Create scene
    book1_final_scene_creation(scene, true);
}

void Raytracing::checkered_spheres(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 20;
    camera.lookfrom = point3(13, 2, 3);
    camera.lookat = point3(0, 0, 0);

    // Scene settings
    scene.name = "Checkered Spheres";
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 100;

    // Materials
    auto checker = make_shared<CheckerTexture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    // Spheres
    auto sphere1 = make_shared<Sphere>(point3(0, -10, 0), 10, make_shared<Lambertian>(checker));
    auto sphere2 = make_shared<Sphere>(point3(0, 10, 0), 10, make_shared<Lambertian>(checker));

    // Add primitives
    scene.add(sphere1);
    scene.add(sphere2);
}

void Raytracing::earth(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 20;
    camera.lookfrom = point3(0, 0, 12);
    camera.lookat = point3(0, 0, 0);

    // Scene settings
    scene.name = "Earth";
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 100;

    // Textures
    auto earth_texture = make_shared<ImageTexture>("earthmap.jpg");

    // Materials
    auto earth_surface = make_shared<Lambertian>(earth_texture);

    // Spheres
    auto globe = make_shared<Sphere>(point3(0, 0, 0), 2, earth_surface);

    // Add primitives
    scene.add(globe);
}

void Raytracing::perlin_spheres(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 20;
    camera.lookfrom = point3(13, 2, 3);
    camera.lookat = point3(0, 0, 0);

    // Scene settings
    scene.name = "Perlin Spheres";
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 100;

    // Textures
    auto perlin_texture = make_shared<NoiseTexture>(4, 7);

    // Materials
    auto perlin_material = make_shared<Lambertian>(perlin_texture);

    // Spheres
    auto sphere1 = make_shared<Sphere>(point3(0, -1000, 0), 1000, perlin_material);
    auto sphere2 = make_shared<Sphere>(point3(0, 2, 0), 2, perlin_material);

    // Add primitives
    scene.add(sphere1);
    scene.add(sphere2);
}

void Raytracing::quads_scene(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 80;
    camera.lookfrom = point3(0, 0, 9);
    camera.lookat = point3(0, 0, 0);

    // Scene settings
    scene.name = "Quads";
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 100;

    // Materials
    auto left_red = make_shared<Lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<Lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<Lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<Lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<Lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    auto quad1 = make_shared<Quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red);
    auto quad2 = make_shared<Quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green);
    auto quad3 = make_shared<Quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue);
    auto quad4 = make_shared<Quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange);
    auto quad5 = make_shared<Quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal);

    // Add primitives
    scene.add(quad1);
    scene.add(quad2);
    scene.add(quad3);
    scene.add(quad4);
    scene.add(quad5);
}

void Raytracing::simple_light(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 20;
    camera.lookfrom = point3(26, 3, 6);
    camera.lookat = point3(0, 2, 0);

    // Scene settings
    scene.name = "Simple Light";
    scene.sky_blend = false;
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 100;

    // Textures
    auto perlin_texture = make_shared<NoiseTexture>(4, 7);

    // Materials
    auto perlin_material = make_shared<Lambertian>(perlin_texture);
    auto diffuse_light_material = make_shared<DiffuseLight>(color(4, 4, 4));

    // Spheres
    auto sphere1 = make_shared<Sphere>(point3(0, -1000, 0), 1000, perlin_material);
    auto sphere2 = make_shared<Sphere>(point3(0, 2, 0), 2, perlin_material);
    auto sphere3 = make_shared<Sphere>(point3(0, 7, 0), 2, diffuse_light_material);

    // Quads
    auto quad1 = make_shared<Quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), diffuse_light_material);

    // Add primitives
    scene.add(sphere1);
    scene.add(sphere2);
    scene.add(sphere3);
    scene.add(quad1);
}

void Raytracing::cornell_box(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 40;
    camera.lookfrom = point3(278, 278, -800);
    camera.lookat = point3(278, 278, 0);

    // Scene settings
    scene.name = "Cornell Box";
    scene.sky_blend = false;
    scene.bounce_max_depth = 10;
    scene.samples_per_pixel = 50;

    // Materials
    auto red = make_shared<Lambertian>(color(.65, .05, .05));
    auto white = make_shared<Lambertian>(color(.73, .73, .73));
    auto green = make_shared<Lambertian>(color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(color(15, 15, 15));
    auto aluminum = make_shared<Metal>(color(0.8, 0.85, 0.88), 0.0);
    auto glass = make_shared<Dielectric>(1.5);

    // Quads
    auto quad1 = make_shared<Quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green);
    auto quad2 = make_shared<Quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red);
    auto quad3 = make_shared<Quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light);
    auto quad4 = make_shared<Quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white);
    auto quad5 = make_shared<Quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white);
    auto quad6 = make_shared<Quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white);

    // Boxes
    auto box1 = make_shared<Box>(point3(0, 0, 0), point3(165, 330, 165), white);
    auto box2 = make_shared<Box>(point3(0, 0, 0), point3(165, 165, 165), white);

    // Transformations
    box1->translate(vec3(265, 0, 295));
    box2->translate(vec3(130, 0, 65));
    box1->rotate(y_axis, 15.0);
    box2->rotate(y_axis, -18.0);

    // Glass Sphere
    auto sphere1 = make_shared<Sphere>(point3(190, 90, 190), 90, glass);

    // Mesh
    auto mesh = load_obj("cube\\cube.obj");

    // Add primitives
    scene.add(quad1);
    scene.add(quad2);
    scene.add(quad3);
    scene.add(quad4);
    scene.add(quad5);
    scene.add(quad6);
    scene.add(box1);
    scene.add(box2);
    // scene.add(sphere1);
    if (mesh) scene.add(mesh);
}

void Raytracing::cornell_smoke(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 40;
    camera.lookfrom = point3(278, 278, -800);
    camera.lookat = point3(278, 278, 0);

    // Scene settings
    scene.name = "Cornell Smoke";
    scene.sky_blend = false;
    scene.bounce_max_depth = 50;
    scene.samples_per_pixel = 200;

    auto red = make_shared<Lambertian>(color(.65, .05, .05));
    auto white = make_shared<Lambertian>(color(.73, .73, .73));
    auto green = make_shared<Lambertian>(color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(color(7, 7, 7));

    auto quad1 = make_shared<Quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green);
    auto quad2 = make_shared<Quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red);
    auto quad3 = make_shared<Quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light);
    auto quad4 = make_shared<Quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white);
    auto quad5 = make_shared<Quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white);
    auto quad6 = make_shared<Quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white);

    // Boxes
    shared_ptr<Hittable> box1 = make_shared<Box>(point3(0, 0, 0), point3(165, 330, 165), white);
    shared_ptr<Hittable> box2 = make_shared<Box>(point3(0, 0, 0), point3(165, 165, 165), white);

    // Transformations
    box1->translate(vec3(265, 0, 295));
    box2->translate(vec3(130, 0, 65));
    box1->rotate(y_axis, 15.0);
    box2->rotate(y_axis, -18.0);

    // Constant medium
    box1 = make_shared<constant_medium>(box1, 0.01, color(0, 0, 0));
    box2 = make_shared<constant_medium>(box2, 0.01, color(1, 1, 1));

    // Add primitives
    scene.add(quad1);
    scene.add(quad2);
    scene.add(quad3);
    scene.add(quad4);
    scene.add(quad5);
    scene.add(quad6);
    scene.add(box1);
    scene.add(box2);
}

void Raytracing::book2_final_scene(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 40;
    camera.lookfrom = point3(478, 278, -600);
    camera.lookat = point3(278, 278, 0);

    // Scene settings
    scene.name = "Book 2 Final Scene";
    scene.sky_blend = false;
    scene.bounce_max_depth = 40;
    scene.samples_per_pixel = 100;
    int boxes_per_side = 20;
    int number_of_spheres = 1000;

    // Textures
    auto earth_texture = make_shared<ImageTexture>("earthmap.jpg");
    auto perlin_texture = make_shared<NoiseTexture>(0.2, 7);

    // Materials
    auto ground = make_shared<Lambertian>(color(0.48, 0.83, 0.53));
    auto light = make_shared<DiffuseLight>(color(7, 7, 7));
    auto sphere_material = make_shared<Lambertian>(color(0.7, 0.3, 0.1));
    auto dielectric_material = make_shared<Dielectric>(1.5);
    auto metal_material = make_shared<Metal>(color(0.8, 0.8, 0.9), 1.0);
    auto white_material = make_shared<Lambertian>(color(.73, .73, .73));
    auto perlin_material = make_shared<Lambertian>(perlin_texture);
    auto earth_surface = make_shared<Lambertian>(earth_texture);

    // Create ground boxes
    hittable_list boxes;

    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_number<double>(1, 101);
            auto z1 = z0 + w;

            auto box = make_shared<Box>(point3(x0, y0, z0), point3(x1, y1, z1), ground);

            boxes.add(box);
        }
    }

    auto box_bvh_tree = make_shared<bvh_node>(boxes);

    // Light source
    auto quad1 = make_shared<Quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light);

    // Material spheres
    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere1 = make_shared<Sphere>(center1, center2, 50, sphere_material);
    auto sphere2 = make_shared<Sphere>(point3(260, 150, 45), 50, dielectric_material);
    auto sphere3 = make_shared<Sphere>(point3(0, 150, 145), 50, metal_material);

    // Texture spheres
    auto sphere4 = make_shared<Sphere>(point3(400, 200, 400), 100, earth_surface);

    // Perlin noise spheres
    auto sphere5 = make_shared<Sphere>(point3(220, 280, 300), 80, perlin_material);

    // Constant medium spheres
    auto boundary1 = make_shared<Sphere>(point3(360, 150, 145), 70, dielectric_material);
    auto sphere6 = make_shared<constant_medium>(boundary1, 0.2, color(0.2, 0.4, 0.9));
    auto boundary2 = make_shared<Sphere>(point3(0, 0, 0), 5000, dielectric_material);
    auto sphere7 = make_shared<constant_medium>(boundary2, 0.0001, color(1, 1, 1));

    // Transformed spheres
    hittable_list spheres;
    shared_ptr<bvh_node> spheres_bvh_tree;
    for (int j = 0; j < number_of_spheres; j++)
    {
        auto sphere = make_shared<Sphere>(point3::random(0, 165), 10, white_material);
        spheres.add(sphere);
    }

    spheres_bvh_tree = make_shared<bvh_node>(spheres);
    spheres_bvh_tree->translate(vec3(-100, 270, 395));
    spheres_bvh_tree->rotate(y_axis, 15);

    // Add objects to the scene
    scene.add(box_bvh_tree);
    scene.add(quad1);
    scene.add(sphere1);
    scene.add(sphere2);
    scene.add(sphere3);
    scene.add(sphere4);
    scene.add(sphere5);
    scene.add(sphere6);
    scene.add(sphere7);
    scene.add(boundary1);
    scene.add(spheres_bvh_tree);
}

void Raytracing::obj_test(Scene& scene, Camera& camera, ImageWriter& image)
{
    // Camera settings
    camera.vertical_fov = 20;
    camera.lookfrom = point3(13, 2, -3);
    camera.lookat = point3(0, 0, 0);
    camera.defocus_angle = 0.6;

    // Scene settings
    scene.name = "OBJ test";
    scene.sky_blend = true;
    scene.bounce_max_depth = 5;
    scene.samples_per_pixel = 10;

    // Mesh
    auto mesh = load_obj("cube\\cube.obj");

    // Add objects to scene
    if (mesh) scene.add(mesh);
}
