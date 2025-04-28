#include "engine/raytracing_engine.hpp"
#include "graphics/raytracing_renderer.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/bind.h>

// Binding code
EMSCRIPTEN_BINDINGS(_Class_) {

    emscripten::class_<RayTracingEngine>("Engine")
        .constructor<>()
        .class_function("getInstance", &RayTracingEngine::get_sample_instance, emscripten::return_value_policy::reference())
        .function("setWasmModuleInitialized", &RayTracingEngine::set_wasm_module_initialized);

}
#endif

int main()
{
    RayTracingEngine* engine = new RayTracingEngine();
    RayTracingRenderer* renderer = new RayTracingRenderer();

    sEngineConfiguration configuration;
    configuration.window_width = 640;
    configuration.window_height = 360;

    if (engine->initialize(renderer, configuration))
        return 1;

    engine->start_loop();
    engine->clean();

    delete engine;
    delete renderer;

    return 0;
}
