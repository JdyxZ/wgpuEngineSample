// Headers
#include "core/core.hpp"
#include "raytracing_engine.hpp"
#include "utils/entity_parser.hpp"

#include "framework/nodes/environment_3d.h"
#include "graphics/raytracing_renderer.hpp"
#include "graphics/renderer_storage.h"
#include "framework/parsers/parse_gltf.h"
#include "engine/scene.h"

int RayTracingEngine::initialize(Renderer* renderer, sEngineConfiguration configuration)
{
	int error = Engine::initialize(renderer, configuration);

    if (error) return error;

    return error;
}

int RayTracingEngine::post_initialize()
{
    main_scene = new Scene("main_scene");

    vector<Node*> scene_nodes;

    GltfParser gltf_parser;
    gltf_parser.parse("models/github_assets/DamagedHelmet/glTF-Binary/DamagedHelmet.glb", scene_nodes, PARSE_GLTF_FILL_SURFACE_DATA);
    // gltf_parser.parse("models/github_assets/Avocado/glTF-Binary/Avocado.glb", scene_nodes, PARSE_GLTF_FILL_SURFACE_DATA);
    // gltf_parser.parse("models/github_assets/Corset/glTF-Binary/Corset.glb", scene_nodes, PARSE_GLTF_FILL_SURFACE_DATA);

    vector<shared_ptr<Raytracing::Mesh>> meshes = parse_nodes(scene_nodes);

    main_scene->add_nodes(scene_nodes);

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
    show_imgui = true;

    if (show_imgui)
        render_gui();

    main_scene->render();

    Engine::render();
}

void RayTracingEngine::render_gui()
{
    bool active = true;

    ImGui::Begin("Debug panel", &active, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    bool scene_tab_open = false;

    if (ImGui::BeginTabBar("TabBar", tab_bar_flags))
    {

        if (ImGui::BeginTabItem("RayTracer"))
        {


            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    render_default_gui();
}
