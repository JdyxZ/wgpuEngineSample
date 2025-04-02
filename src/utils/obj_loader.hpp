#pragma once

// Headers
#include "core/core.hpp"

// Namespace forward declarations
namespace Raytracing
{
    class Mesh;
}

shared_ptr<Raytracing::Mesh> load_obj(const string& filename);

