#pragma once

// Raytracing Headers
#include "graphics/raytracing_renderer.hpp"

// Framework Headers
#include "engine/engine.h"

class Node3D;

class RayTracingEngine : public Engine
{
private:

    bool show_imgui = true;
    bool is_raytracer_rendering = false;
    Raytracing::RendererSettings settings;

public:

	int initialize(Renderer* renderer, sEngineConfiguration configuration = {}) override;
    int post_initialize() override;
    void clean() override;
	void update(float delta_time) override;
	void render() override;
    void create_scene();
    void render_gui();

    static RayTracingEngine* get_sample_instance() { return static_cast<RayTracingEngine*>(instance); }

    #ifdef __EMSCRIPTEN__
    void set_wasm_module_initialized(bool value) {
        wasm_module_initialized = value;
    }
    #endif
};
