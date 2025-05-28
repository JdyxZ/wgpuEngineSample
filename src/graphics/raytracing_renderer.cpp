// Raytracing Headers
#include "core/core.hpp"
#include "raytracing_renderer.hpp"
#include "scene.hpp"
#include "graphics/camera.hpp"
#include "utils/image_writer.hpp"
#include "utils/log_writer.hpp"

// Framework Headers
#include "glm/gtx/norm.hpp"
#include "shaders/mesh_forward.wgsl.gen.h"
#include "framework/nodes/mesh_instance_3d.h"
#include "framework/camera/camera_2d.h"
#include "graphics/renderer_storage.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

// Usings
using Raytracing::Scene;
using Raytracing::ImageWriter;
using Raytracing::RendererSettings;
using Raytracing::CameraData;
using Raytracing::SkyboxTexture;

RayTracingRenderer::RayTracingRenderer(const sRendererConfiguration& config) : Renderer(config) {}

int RayTracingRenderer::pre_initialize(GLFWwindow* window, bool use_mirror_screen)
{
    return Renderer::pre_initialize(window, use_mirror_screen);
}

int RayTracingRenderer::initialize()
{
    int error_code = Renderer::initialize();

    return error_code;
}

int RayTracingRenderer::post_initialize()
{
    Renderer::post_initialize();

    clear_color = glm::vec4(0.22f, 0.22f, 0.22f, 1.0);

    Texture* irradiance = RendererStorage::get_texture("data/textures/environments/sky.hdr", TEXTURE_STORAGE_STORE_DATA);
    set_irradiance_texture(irradiance);

    return 0;
}

void RayTracingRenderer::clean()
{
    Renderer::clean();
}

void RayTracingRenderer::update(float delta_time)
{
    Renderer::update(delta_time);
}

void RayTracingRenderer::render()
{
    if (manual_scene)
        screen_mesh->render();

    Renderer::render();
}

void RayTracingRenderer::resize_window(int width, int height)
{
    Renderer::resize_window(width, height);

    if (manual_scene)
    {
        camera_2d->set_view(glm::mat4x4(1.0f));
        camera_2d->set_projection(glm::mat4x4(1.0f));

        screen_texture->create(WGPUTextureDimension_2D, WGPUTextureFormat_RGBA8UnormSrgb, { webgpu_context->screen_width, webgpu_context->screen_height, 1 }, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, 1, 1, nullptr);
        screen_mesh->get_surface_material(0)->set_dirty_flag(PROP_DIFFUSE_TEXTURE);

        image = ImageWriter(webgpu_context->screen_width, webgpu_context->screen_height);
        image.initialize();

        camera.initialize(scene, image);
        camera.render(scene, image);

        screen_texture->update(image.get_ubyte_data().data(), 0, {});
        image.save();
    }
}

void RayTracingRenderer::render_manual_scene()
{
    // Set render type
    manual_scene = true;

    // Set 2D camera to generate the frame
    camera_2d->set_view(glm::mat4x4(1.0f));
    camera_2d->set_projection(glm::mat4x4(1.0f));

    // Create quad mesh to show gpu texture on window
    Surface* screen_surface = new Surface();
    screen_mesh = new MeshInstance3D();
    screen_surface->create_quad(2.0f, 2.0f);
    screen_mesh->add_surface(screen_surface);

    // GPU texture to store the generated frame
    screen_texture = new Texture();
    screen_texture->create(WGPUTextureDimension_2D, WGPUTextureFormat_RGBA8UnormSrgb, { webgpu_context->screen_width, webgpu_context->screen_height, 1 }, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, 1, 1, nullptr);

    // Material to show generated frame on window
    Material* screen_material = new Material();
    screen_material->set_is_2D(true);
    screen_material->set_depth_read(false);
    screen_material->set_depth_write(false);
    screen_material->set_type(MATERIAL_UNLIT);
    screen_material->set_diffuse_texture(screen_texture);
    screen_material->set_shader(RendererStorage::get_shader_from_source(shaders::mesh_forward::source, shaders::mesh_forward::path, shaders::mesh_forward::libraries, screen_material));
    screen_surface->set_material(screen_material);

    // Log space
    std::cout << std::endl;

    // Scene start
    scene.start();

    // Build scene
    scene.build(camera, image);

    // Create image
    image = ImageWriter(webgpu_context->screen_width, webgpu_context->screen_height);

    // Intialize image
    image.initialize();

    // Initialize the camera
    camera.initialize(scene, image);

    // Render scene
    camera.render(scene, image);

    // Update framebuffer
    screen_texture->update(image.get_ubyte_data().data(), 0, {});

    // Encode and save image with desired format
    image.save();

    // Scene end
    scene.end();

    // Write scene log
    log.write(scene, camera, image);
}

void RayTracingRenderer::render_frame(const ParsedScene& parsed_scene, const RendererSettings& settings)
{
    // Unwrap parsed scene 
    auto meshes = parsed_scene.meshes;

    // Create scene
    Scene scene;

    // Create camera
    Raytracing::Camera camera;

    // Create image
    ImageWriter image = ImageWriter(settings.screen_width, settings.screen_height);

    // Init
    image.initialize(settings);
    scene.initialize(settings);
    camera.initialize(settings, scene, image);

    // Scene start
    scene.start();

    // Build scene
    scene.build(meshes);

    // Render scene
    camera.render(scene, image);

    // Encode and save image with desired format
    image.save();

    // Scene end
    scene.end();

    // Write scene log
    LogWriter log;
    log.write(scene, camera, image);
}

// Static attributes
float RayTracingRenderer::render_progress = 0.f;
string RayTracingRenderer::last_render_image = "";
