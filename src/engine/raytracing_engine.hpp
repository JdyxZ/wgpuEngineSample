#pragma once

// Raytracing Headers
#include "graphics/raytracing_renderer.hpp"
#include "utils/project_parsers.hpp"

// Framework Headers
#include "engine/engine.h"

// Forward declaration
class Node3D;

class RayTracingEngine : public Engine
{
private:

    bool show_imgui = true;

    Raytracing::RendererSettings settings;
    RayTracingRenderer* renderer = nullptr;

    std::jthread render_jthread;

public:

	int initialize(Renderer* renderer, sEngineConfiguration configuration = {}) override;
    int post_initialize() override;
    void clean() override;
	void update(float delta_time) override;
	void render() override;
    void create_scene();
    void render_gui();
    void render_frame(const ParsedScene& scene, const Raytracing::RendererSettings& settings, std::stop_token s_token) const;

    static RayTracingEngine* get_sample_instance() { return static_cast<RayTracingEngine*>(instance); }

    #ifdef __EMSCRIPTEN__
    void set_wasm_module_initialized(bool value) {
        wasm_module_initialized = value;
    }
    #endif
};
