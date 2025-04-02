// Headers
#include "core/core.hpp"
#include "scene.hpp"
#include "utils/utilities.hpp"
#include "utils/chrono.hpp"
#include "hittables/hittable.hpp"
#include "hittables/box.hpp"
#include "hittables/quad.hpp"
#include "hittables/mesh.hpp"
#include "materials/material.hpp"
#include "graphics/camera.hpp"
#include "utils/image_writer.hpp"
#include "scenes.hpp"
#include "bvh.hpp"

// Usings
using Raytracing::Mesh;
using Raytracing::Material;
using Raytracing::Camera;
using Raytracing::ImageWriter;

Raytracing::Scene::Scene()
{
    full_pipeline = make_shared<Chrono>();
    build_chrono = make_shared<Chrono>();
    bvh_chrono = make_shared<Chrono>();
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

    switch (object->get_type())
    {
    case SPHERE:
        spheres++;
        primitives++;
        break;
    case TRIANGLE:
        triangles++;
        primitives++;
        break;
    case BOX:
    {
        auto box_ptr = std::dynamic_pointer_cast<Box>(object);

        *bvh_chrono += *box_ptr->bvh_chrono();

        quads += 6;
        primitives += 6;
        break;
    }
    case QUAD:
    {
        auto quad_ptr = std::dynamic_pointer_cast<Quad>(object);

        auto material = quad_ptr->get_material();
        if (material->get_type() == DIFFUSE_LIGHT) emissives++;

        quads++;
        primitives++;
        break;
    }
    case MESH:
    {
        auto mesh_ptr = std::dynamic_pointer_cast<Mesh>(object);
        meshes.push_back(mesh_ptr);

        auto mesh_triangles = mesh_ptr->num_triangles();
        triangles += mesh_triangles;
        primitives += mesh_triangles;

        *bvh_chrono += *mesh_ptr->bvh_chrono();
        break;
    }
    case BVH_NODE:
    {
        auto bvh_node_ptr = std::dynamic_pointer_cast<bvh_node>(object);
        *bvh_chrono += *bvh_node_ptr->bvh_chrono();
        break;
    }
    }
}

void Raytracing::Scene::build(Camera& camera, ImageWriter& image)
{
    // Log info
    Logger::info("MAIN", "Scene build started.");

    // Start scene build time chrono
    this->build_chrono->start();

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
    bvh_depth = BVH_tree->depth;
    bvh_nodes = BVH_tree->nodes;

    // End scene build time chrono
    build_chrono->end();

    // Info log
    Logger::info("Main", "Scene build completed.");
}
