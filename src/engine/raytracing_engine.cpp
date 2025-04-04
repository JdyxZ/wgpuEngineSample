// Headers
#include "core/core.hpp"
#include "raytracing_engine.hpp"
#include "framework/nodes/environment_3d.h"
#include "graphics/raytracing_renderer.hpp"
#include "graphics/renderer_storage.h"
#include "engine/scene.h"

int RayTracingEngine::initialize(Renderer* renderer, sEngineConfiguration configuration)
{
	int error = Engine::initialize(renderer, configuration);

    if (error) return error;

    main_scene = new Scene("main_scene");

    return error;
}

int RayTracingEngine::post_initialize()
{
    return Engine::post_initialize();
}

void RayTracingEngine::clean()
{
    Engine::clean();
}

void RayTracingEngine::update(float delta_time)
{
    Engine::update(delta_time);

    main_scene->update(delta_time);
}

void RayTracingEngine::render()
{
    show_imgui = false;

    if (show_imgui)
        render_default_gui();

    main_scene->render();

    Engine::render();
}
