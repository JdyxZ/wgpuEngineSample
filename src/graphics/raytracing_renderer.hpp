#pragma once

// Raytracing Headers
#include "core/core.hpp"
#include "includes.h"
#include "graphics/renderer.h"
#include "scene.hpp"
#include "camera.hpp"
#include "utils/image_writer.hpp"
#include "utils/log_writer.hpp"
#include "math/vec3.hpp"
#include "hittables/mesh.hpp"
#include "utils/project_parsers.hpp"

// Namespace forward declarations
namespace Raytracing
{
    class SkyboxTexture;
}

namespace Raytracing
{
    struct RendererSettings
    {
        // Render
        int bounce_max_depth = 50;
        float min_hit_distance = 0.001f;
        int samples_per_pixel = 100;

        // Optimizations
        bool bvh_optimization = true;
        bool russian_roulette = false;
        bool parallel_computing = true;

        // Background
        BACKGROUND_TYPE background_type = BACKGROUND_TYPE::SKYBOX;
        ImVec4 background_color = ImVec4(0.22f, 0.22f, 0.22f, 1.0);
        ImVec4 primary_blend_color = ImVec4(float(WHITE.x), float(WHITE.y), float(WHITE.z), 1.0);
        ImVec4 secondary_blend_color = ImVec4(float(SKY_BLUE.x), float(SKY_BLUE.y), float(SKY_BLUE.z), 1.0);
        Raytracing::SkyboxTexture* skybox = nullptr;

        // Screen
        int screen_width = 0;
        int screen_height = 0;

        // Camera
        CameraData camera_data;

        // Output              
        IMAGE_FORMAT format = PNG_8;
        int quality = 100;
        string image_path = output_path;

    };
}

class RayTracingRenderer : public Renderer
{
private:

    // Render items
    Raytracing::Scene scene;
    Raytracing::Camera camera;
    Raytracing::ImageWriter image;
    LogWriter log;

    // Screen (for manual render)
    MeshInstance3D* screen_mesh = nullptr;
    Texture* screen_texture = nullptr;

    // Render
    void render_manual_scene();

    // Aux vars
    bool manual_scene = false;

public:

    RayTracingRenderer(const sRendererConfiguration& config = {});

    static float render_progress;
    static string last_render_image;

    int pre_initialize(GLFWwindow* window, bool use_mirror_screen = false) override;
    int initialize() override;
    int post_initialize() override;

    void clean() override;
    void update(float delta_time) override;
    void render() override;

    virtual void resize_window(int width, int height) override;
    void render_frame(const ParsedScene& scene, const Raytracing::RendererSettings& settings);
};
