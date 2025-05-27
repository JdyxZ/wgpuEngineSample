// Directives
#pragma execution_character_set("utf-8")

// Headers
#include "core/core.hpp"
#include "log_writer.hpp"
#include "utilities.hpp"
#include "project_info.hpp"
#include "system_info.hpp"
#include "scene.hpp"
#include "chrono.hpp"
#include "image_writer.hpp"
#include "graphics/camera.hpp"
#include "hittables/mesh.hpp"
#include "utils/scene_stats.hpp"

// Usings
using Raytracing::Scene;
using Raytracing::Camera;
using Raytracing::ImageWriter;

LogWriter::LogWriter()
{
    // Create directory for logs in case it does not already exist
    string folderPath = logs_directory + "\\";
    fs::create_directories(folderPath);
}

void LogWriter::write(const Scene& scene, const Camera& camera, ImageWriter& image)
{
    // Define name based on timestamp
    string name = get_current_timestamp();

    // Log info
    Logger::info("LogWriter", "Writing log file started.");

    // Create log
    std::ostringstream log;

    log << "# Render Log 📜\n\n";

    // System Section
    log << "## System 🖥️\n\n";
    log << "**Platform:** " << SystemInfo::platform << "  \n";
    log << "**CPU Threads Used:** " << SystemInfo::cpu_threads << "  \n\n";

    // Project Section
    log << "## Project 🏯\n\n";
    log << "**Raytracer Version:** " << ProjectInfo::version << "  \n";
    log << "**Build Configuration:** " << ProjectInfo::build_configuration << "  \n";
    log << "**Compiler:** " << ProjectInfo::compiler << "  \n";
    log << "**Start Time:** " << scene._start << "  \n";
    log << "**End Time:** " << scene._end << "  \n";
    log << "**Elapsed time:** " << scene.full_pipeline.elapsed_to_string() << "\n\n";

    // Camera Section
    log << "## Camera 📷\n\n";
    log << "**Type:** Perspective  \n";
    log << "**Position:** " << camera.lookfrom << " \n";
    log << "**LookAt:** " << camera.lookat << " \n";
    log << "**Up Vector:** " << camera.world_up << " \n";
    log << "**FOV:** " << camera.vertical_fov << "  \n";
    log << "**Defocus Angle:** " << camera.defocus_angle << "  \n";
    log << "**Focus Distance:** " << camera.focus_distance << "  \n\n";

    // Scene Section
    log << "## Scene 🌆\n\n";
    log << "**Name:** " << scene.name << "  \n";
    log << "**Background Color:** " << scene.background << " \n";
    log << "**Samples per Pixel:** " << scene.samples_per_pixel << "  \n";
    log << "**Max Ray Bounces:** " << scene.bounce_max_depth << "  \n";
    log << "**Build Time:** " << scene.build_chrono.elapsed_to_string() << "\n\n";

    // BVH Section
    log << "## BVH 🍂\n\n";
    log << "**Depth:** " << scene.stats.bvh_depth << "  \n";
    log << "**Nodes:** " << scene.stats.bvh_nodes << "  \n";
    log << "**BVHs Build Time:** " << scene.stats.bvh_chrono.elapsed_to_string() << "\n\n";

    // Primitives
    log << "## Primitives 🔵\n\n";
    log << "**Spheres:** " << scene.stats.spheres << "  \n";
    log << "**Quads:** " << scene.stats.quads << "  \n";
    log << "**Triangles:** " << scene.stats.triangles << "  \n";
    log << "**Total:** " << scene.stats.primitives << "  \n\n";

    // Meshes
    log << "## Meshes 🔺\n\n";
    for (auto mesh : scene.stats.meshes)
    {
        auto mesh_bvh_stats = mesh->get_stats();
        log << "· `" << mesh->name() << "`:\n";
        log << "    - **Total Triangles:** " << mesh_bvh_stats.triangles << "  \n";
        log << "    - **Surfaces:** " << mesh->num_surfaces() << "  \n";
        log << "    - **BVH build time:** " << mesh_bvh_stats.bvh_chrono.elapsed_to_string() << " \n";
    }
    log << "\n";

    // Image Section
    log << "## Image 🖼️\n\n";
    log << "**Name:** `" << image.full_name << "`  \n";
    log << "**Resolution:** " << image.get_width() << "x" << image.get_height() << "  \n";
    log << "**Color precision:** " << image.get_precision() << " bits  \n";
    log << "**File Size:** " << image.size.amount << " " << image.size.unit << "  \n";
    log << "**Encoding Time:** " << image.encoding_chrono.elapsed_to_string() << " \n\n";

    // Render Benchmark
    log << "## Render Benchmark 🎇\n\n";
    log << "**Rendering Time:** " << camera.render_chrono.elapsed_to_string() << " \n";
    log << "**Rays:**\n";
    log << "    - **Primary Rays:** " << camera.primary_rays << "  \n";
    log << "    - **Background Rays:** " << camera.background_rays << "  \n";
    log << "    - **Light Rays:** " << camera.light_rays << "  \n";
    log << "    - **Reflected Rays:** " << camera.reflected_rays << "  \n";
    log << "    - **Refracted Rays:** " << camera.refracted_rays << "  \n";
    log << "    - **Unknwon Rays:** " << camera.unknwon_rays << "  \n";
    log << "    - **Total Rays Casted:** " << camera.rays_casted << "  \n";
    log << "    - **Average Rays per Second:** " << camera.average_rays_per_second << "  \n\n";

    // Log messages
    log << "## Log 📋\n\n";
    for (auto message : Logger::messages())
        log << message.str();

    // Construct the log file
    std::string log_filename = logs_path + name + ".md";
    std::ofstream log_file(log_filename);

    if (!log_file)
        Logger::error("LogWriter", "Could not open log file for writing: " + name + ".md");

    // Write log content to file
    log_file << log.str();
    log_file.close();
    Logger::info("LogWriter", "Log file saved: " + name + ".md");

    // Clear log messages in case of multiple scene render
    Logger::clear();
}

