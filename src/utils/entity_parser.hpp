#pragma once

// Headers
#include "core/core.hpp"
#include "framework/nodes/mesh_instance_3d.h"

// Namespace forward declarations
namespace Raytracing
{
    class Mesh;
    class Surface;
}

vector<shared_ptr<Raytracing::Mesh>> parse_nodes(const vector<Node*>& nodes);
vector<shared_ptr<Raytracing::Mesh>> parse_node(Node* node);
shared_ptr<Raytracing::Surface> parse_surface(Surface* surface);
