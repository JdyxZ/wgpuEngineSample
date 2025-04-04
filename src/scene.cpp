// Headers
#include "core/core.hpp"
#include "scene.hpp"
#include "utils/utilities.hpp"
#include "utils/chrono.hpp"
#include "hittables/hittable.hpp"
#include "graphics/camera.hpp"
#include "utils/image_writer.hpp"
#include "scene_stats.hpp"
#include "scenes.hpp"
#include "hittables/bvh.hpp"

// Usings
using Raytracing::Camera;
using Raytracing::ImageWriter;

Raytracing::Scene::Scene()
{
    stats = make_shared<scene_stats>();
    full_pipeline = make_shared<Chrono>();
    build_chrono = make_shared<Chrono>();
}

void Raytracing::Scene::start()
{
    _start = get_current_timestamp("%c");
    full_pipeline->start();
}

void Raytracing::Scene::end()
{
    _end = get_current_timestamp("%c");
    full_pipeline->end();
}

void Raytracing::Scene::add(shared_ptr<Hittable> object)
{
    hittable_list::add(object);

    if (object->has_pdf())
        hittables_with_pdf.push_back(object);

    stats->add(object);
}

void Raytracing::Scene::clear()
{
    hittable_list::clear();
    stats.reset(new scene_stats());
}

void Raytracing::Scene::build(Camera& camera, ImageWriter& image)
{
    // Log info
    Logger::info("MAIN", "Scene build started.");

    // Start scene build time chrono
    this->build_chrono->start();

    // Choose rendering scene
    switch (9)
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
    build_chrono->end();

    // Info log
    Logger::info("Main", "Scene build completed.");
}
