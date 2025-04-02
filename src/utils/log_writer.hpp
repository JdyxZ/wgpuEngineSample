#pragma once

// Headers
#include "core/core.hpp"

// Namespace forward declarations
namespace Raytracing
{
    class Scene;
    class Camera;
    class ImageWriter;
}

struct LogWriter
{
    LogWriter();

    void write(const Raytracing::Scene& scene, const Raytracing::Camera& camera, Raytracing::ImageWriter& image);
};

