// Headers
#include "core/core.hpp"
#include "scene_stats.hpp"
#include "utils/chrono.hpp"
#include "hittables/box.hpp"
#include "hittables/quad.hpp"
#include "hittables/mesh.hpp"
#include "materials/material.hpp"
#include "bvh.hpp"

// Usings
using Raytracing::Mesh;
using Raytracing::Material;

scene_stats::scene_stats()
{
    bvh_chrono = make_shared<Chrono>();
}

void scene_stats::add(const shared_ptr<Hittable> object)
{
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
        *this += *box_ptr->stats();
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
        *this += *mesh_ptr->stats();
        break;
    }
    case BVH_NODE:
    {
        auto bvh_node_ptr = std::dynamic_pointer_cast<bvh_node>(object);
        auto stats = bvh_node_ptr->get_stats();

        if (stats == nullptr)
        {
            string error = Logger::error("BVH_NODE", "BVH node stats is null!!!");
            throw std::runtime_error(error);
        }

        *this += *stats;
       
        break;
    }
    }
}

scene_stats& scene_stats::operator+=(const scene_stats& s)
{
    spheres += s.spheres;
    quads += s.quads;
    triangles += s.triangles;
    primitives += s.primitives;
    emissives += s.emissives;
    bvh_depth += s.bvh_depth;
    bvh_nodes += s.bvh_nodes;
    *bvh_chrono += *s.bvh_chrono;
    return *this;
}

