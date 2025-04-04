#pragma once

// Headers
#include "hittables/hittable_list.hpp"
#include "math/vec3.hpp"
#include "graphics/color.hpp"

// Forward declarations
struct Chrono;
struct scene_stats;

// Namespace forward declarations
namespace Raytracing
{
    class Camera;
    class ImageWriter;
}

namespace Raytracing
{
    class Scene : public hittable_list
    {
    public:

        // Scene properties
        string name;

        // Ray scattering settings
        int bounce_max_depth = 10;          // Maximum number of ray bounces into scene
        double min_hit_distance = 0.001;    // Greatly solves shadow acne

        // Antialiasing and noise settings
        int samples_per_pixel = 10;         // Count of random samples for each pixel

        // Scene background color
        bool sky_blend = true;              // Enables a background sky gradient
        color background = SKY_BLUE;        // Sky gradient primary color

        // Hittable objects with PDF sampling distribution
        vector<shared_ptr<Hittable>> hittables_with_pdf;

        // Stats
        shared_ptr<scene_stats> stats;

        // Timestamps
        string _start;
        string _end;

        // Chronos
        shared_ptr<Chrono> full_pipeline;
        shared_ptr<Chrono> build_chrono;

        // Constructors
        Scene();

        // Methods
        void start();
        void end();
        void add(shared_ptr<Hittable> object) override;
        void clear() override;
        void build(Camera& camera, ImageWriter& image);
    };
}


