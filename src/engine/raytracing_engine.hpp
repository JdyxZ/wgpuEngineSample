#pragma once

#include "engine/engine.h"

class Node3D;

class RayTracingEngine : public Engine
{
public:

	int initialize(Renderer* renderer, sEngineConfiguration configuration = {}) override;
    int post_initialize() override;
    void clean() override;
	void update(float delta_time) override;
	void render() override;
    void render_gui();

    static RayTracingEngine* get_sample_instance() { return static_cast<RayTracingEngine*>(instance); }

    #ifdef __EMSCRIPTEN__
    void set_wasm_module_initialized(bool value) {
        wasm_module_initialized = value;
    }
    #endif
};
