#pragma once

// Headers
#include "core/core.hpp"

// Forward declarations
class Node;
class Surface;
class Camera;

// Namespace forward declarations
namespace Raytracing
{
    class Mesh;
    class Surface;
    struct CameraData;
}

// Object parsers
vector<shared_ptr<Raytracing::Mesh>> parse_nodes(const vector<Node*>& nodes);
vector<shared_ptr<Raytracing::Mesh>> parse_node(Node* node);
shared_ptr<Raytracing::Surface> parse_surface(Surface* surface);

// Data parsers
Raytracing::CameraData parse_camera_data(Camera* camera);
