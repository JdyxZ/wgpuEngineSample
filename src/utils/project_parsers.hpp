#pragma once

// Headers
#include "core/core.hpp"

// Framework headers
#include <webgpu/webgpu.h>

// Forward declarations
class Node;
class Surface;
class Camera;

// Namespace forward declarations
namespace Raytracing
{
    class Mesh;
    class Surface;
    class Matrix44;
    struct CameraData;
}

// Object parsers
vector<shared_ptr<Raytracing::Mesh>> parse_nodes(const vector<Node*>& nodes, const bool use_bvh);
vector<shared_ptr<Raytracing::Mesh>> parse_node(Node* node, const bool use_bvh);
shared_ptr<Raytracing::Surface> parse_surface(Surface* surface, const Raytracing::Matrix44& model, const bool use_bvh);
optional<pair<double, double>> parse_texture_uvs(const optional<pair<double, double>>& uvs, const pair<WGPUAddressMode, WGPUAddressMode>& uv_wrap_mode);
double parse_uv(double coord, WGPUAddressMode wrap_mode);

// Data parsers
Raytracing::CameraData parse_camera_data(Camera* camera);
