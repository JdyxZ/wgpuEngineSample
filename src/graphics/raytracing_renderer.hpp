#pragma once

// Headers
#include "core/core.hpp"
#include "includes.h"
#include "graphics/renderer.h"
#include "scene.hpp"
#include "camera.hpp"
#include "utils/image_writer.hpp"
#include "utils/log_writer.hpp"

class RayTracingRenderer : public Renderer
{
private:

    // Render items
    Raytracing::Scene scene;
    Raytracing::Camera camera;
    Raytracing::ImageWriter image;
    LogWriter log;

    // Screen
    MeshInstance3D* screen_mesh = nullptr;
    Texture* screen_texture = nullptr;

    void init_frame_windows();

    void generate_frame(vector<Raytracing::Mesh> meshes);

public:

    RayTracingRenderer(const sRendererConfiguration& config = {});

    int pre_initialize(GLFWwindow* window, bool use_mirror_screen = false) override;
    int initialize() override;
    int post_initialize() override;
    void clean() override;
    void update(float delta_time) override;
    void render() override;
    virtual void resize_window(int width, int height) override;
};
