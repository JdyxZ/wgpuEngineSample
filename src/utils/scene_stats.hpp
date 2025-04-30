#pragma once

// Headers
#include "core/core.hpp"
#include "chrono.hpp"

// Forward declarations
class Hittable;

// Namespace forward declarations
namespace Raytracing
{
    class Mesh;
}

struct scene_stats
{
    int spheres = 0;
    int quads = 0;
    int triangles = 0;
    int primitives = 0;
    int emissives = 0;
    int bvh_depth = 0;
    int bvh_nodes = 0;
    Chrono bvh_chrono;
    vector<shared_ptr<Raytracing::Mesh>> meshes; // Mesh vector for log support

    scene_stats();

    static int get_bvh_depth(const shared_ptr<Hittable> object);
    void add(const shared_ptr<Hittable> object);

    scene_stats& operator+=(const scene_stats& s);
};

// Aliases
using bvh_stats = scene_stats;
