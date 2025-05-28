#pragma once

// Headers
#include "math/vec3.hpp"
#include "hittables/hittable.hpp"
#include "graphics/color.hpp"
#include "utils/chrono.hpp"
#include "utils/scene_stats.hpp"

// Namespace forward declarations
namespace Raytracing
{
    class Camera;
    class ImageWriter;
    struct RendererSettings;
    class Mesh;
    class SkyboxTexture;
}

enum class BACKGROUND_TYPE
{
    STATIC_COLOR,
    GRADIENT,
    SKYBOX,
};

namespace Raytracing
{
    class Scene : public Hittable
    {
    public:

        // Scene properties
        string name;

        // Ray scattering settings
        int bounce_max_depth = 10;                                          // Maximum number of ray bounces into scene
        double min_hit_distance = 0.001;                                    // Greatly solves shadow acne

        // Optimizations
        bool bvh_optimization = true;                                       // Enables BVH acceleration structure for raytracing
        bool russian_roulette = true;                                       // Enables Russian Roulette for raytracing
        bool parallel_computing = true;                                     // Enables parallel computation throguh OpenMP for raytracing

        // Antialiasing and noise settings
        int samples_per_pixel = 10;                                         // Count of random samples for each pixel

        // Background
        BACKGROUND_TYPE background_type = BACKGROUND_TYPE::STATIC_COLOR;    // Enables a background sky gradient
        color background = BLACK;                                           // Scene background color
        color background_primary = WHITE;                                   // Sky gradient primary color
        color background_secondary = SKY_BLUE;                              // Sky gradient secondary color
        Raytracing::SkyboxTexture* skybox = nullptr;                        // Skybox texture of the background

        // Hittable object
        shared_ptr<Hittable> scene_hittable = nullptr;

        // Hittable vectors
        vector<shared_ptr<Hittable>> scene_hittables;
        vector<shared_ptr<Hittable>> hittables_with_pdf;

        // Stats
        scene_stats stats;

        // Timestamps
        string _start;
        string _end;

        // Chronos
        Chrono full_pipeline;
        Chrono build_chrono;

        // Constructors
        Scene();

        // Methods
        void initialize(const RendererSettings& settings);
        void start();
        void end();

        void add(shared_ptr<Hittable> object);
        void clear();
        size_t size() const;

        void build(Camera& camera, ImageWriter& image);
        void build(vector<shared_ptr<Mesh>> meshes);

        bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;

        void set_bbox();
    };
}


