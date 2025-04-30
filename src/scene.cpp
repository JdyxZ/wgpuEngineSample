// Headers
#include "core/core.hpp"
#include "scene.hpp"
#include "utils/utilities.hpp"
#include "hittables/hittable.hpp"
#include "graphics/camera.hpp"
#include "utils/image_writer.hpp"
#include "scenes.hpp"
#include "hittables/bvh.hpp"
#include "graphics/raytracing_renderer.hpp"

// Usings
using Raytracing::RendererSettings;

Raytracing::Scene::Scene()
{
    stats = scene_stats();
    full_pipeline = Chrono();
    build_chrono = Chrono();
}

void Raytracing::Scene::initialize(RendererSettings& settings)
{
    auto bc = settings.background_color;
    auto cp = settings.primary_blend_color;
    auto cs = settings.primary_blend_color;

    this->bounce_max_depth = settings.bounce_max_depth;
    this->min_hit_distance = settings.min_hit_distance;
    this->samples_per_pixel = settings.samples_per_pixel;
    this->sky_blend = settings.sky_blend;
    this->background = color(bc.x, bc.y, bc.z);
    this->background_primary = color(cp.x, cp.y, cp.z);
    this->background_secondary = color(cs.x, cs.y, cs.z);
}

void Raytracing::Scene::start()
{
    _start = get_current_timestamp("%c");
    full_pipeline.start();
}

void Raytracing::Scene::end()
{
    _end = get_current_timestamp("%c");
    full_pipeline.end();
}

void Raytracing::Scene::add(shared_ptr<Hittable> object)
{
    hittable_list::add(object);

    if (object->has_pdf())
        hittables_with_pdf.push_back(object);

    stats.add(object);
}

void Raytracing::Scene::clear()
{
    hittable_list::clear();
    stats = scene_stats(); // reset stats
}

void Raytracing::Scene::build(Raytracing::Camera& camera, Raytracing::ImageWriter& image)
{
    // Log info
    Logger::info("MAIN", "Scene build started.");

    // Start scene build time chrono
    this->build_chrono.start();

    // Choose rendering scene
    switch (7)
    {
    case 0:
        Raytracing::book1_final_scene(*this, camera, image);
        break;
    case 1:
        Raytracing::bouncing_spheres(*this, camera, image);
        break;
    case 2:
        Raytracing::checkered_spheres(*this, camera, image);
        break;
    case 3:
        Raytracing::earth(*this, camera, image);
        break;
    case 4:
        Raytracing::perlin_spheres(*this, camera, image);
        break;
    case 5:
        Raytracing::quads_scene(*this, camera, image);
        break;
    case 6:
        Raytracing::simple_light(*this, camera, image);
        break;
    case 7:
        Raytracing::cornell_box(*this, camera, image);
        break;
    case 8:
        Raytracing::cornell_smoke(*this, camera, image);
        break;
    case 9:
        Raytracing::book2_final_scene(*this, camera, image);
        break;
    case 10:
        Raytracing::obj_test(*this, camera, image);
        break;
    }

    // Boost scene render with BVH
    auto BVH_tree = make_shared<bvh_node>(*this);
    clear();
    add(BVH_tree);

    // End scene build time chrono
    build_chrono.end();

    // Info log
    Logger::info("Main", "Scene build completed.");
}

void Raytracing::Scene::build(vector<shared_ptr<Raytracing::Mesh>> meshes)
{
    // Log info
    Logger::info("MAIN", "Scene build started.");

    // Start scene build time chrono
    this->build_chrono.start();

    // Add meshes to scene
    for (auto mesh : meshes)
    {
        shared_ptr<Hittable> hittable_mesh = std::dynamic_pointer_cast<Hittable>(mesh);

        if (hittable_mesh)
            this->add(hittable_mesh);
        else
            Logger::error("MAIN", "Mesh cannot be cast to Hittable.");
    }

    // Boost scene render with BVH
    auto BVH_tree = make_shared<bvh_node>(*this);
    clear();
    add(BVH_tree);

    // End scene build time chrono
    build_chrono.end();

    // Info log
    Logger::info("Main", "Scene build completed.");
}
