#pragma once

// Headers
#include "core/core.hpp"
#include "utilities.hpp"
#include "chrono.hpp"

// Namespace forward declarations
namespace Raytracing
{
    struct RendererSettings;
}

enum IMAGE_FORMAT
{
    PNG,
    JPG,
    EXR
};

enum IMAGE_DYNAMIC_RANGE
{
    LDR,
    HDR
};

namespace Raytracing
{
    class ImageWriter
    {
    public:
        string name;                                // Image name
        string full_name;                           // Image name + extension
        IMAGE_FORMAT format = JPG;                  // Image format
        string format_str;                          // Image extension string
        string output_destination = output_path;    // Output path for the image
        int precision = 8;                          // Bit precision of the output image
        int quality = 100;                          // Only for JPG images
        double aspect_ratio;                        // Ratio of image width over height
        file_size size;                             // Output image file size
        Chrono encoding_chrono;                     // Chrono to measure the image encoding time using stb_image_write

        ImageWriter();
        ImageWriter(int width, int height);

        void initialize();
        void initialize(RendererSettings& settings);

        void write_pixel(const int pixel_row, const int pixel_column, const tuple<uint8_t, uint8_t, uint8_t, uint8_t> RGBA_color);
        void write_pixel(const int pixel_row, const int pixel_column, const tuple<float, float, float, float> RGBA_color);
        void save();

        int get_width() const;
        int get_height() const;
        vector<uint8_t> get_rgb_data() const;
        vector<uint8_t> get_rgba_data() const;
        IMAGE_DYNAMIC_RANGE get_dynamic_range() const;

    private:

        // LDR data
        vector<uint8_t> data;                   // Image buffer (RGBA format)

        // Separate RGB channels (TinyEXR requires planar format)
        vector<float> r_data;
        vector<float> g_data;
        vector<float> b_data;
        vector<float> a_data;

        int width;                              // Rendered image width in pixel count
        int height;                             // Rendered image height

        bool savePNG(const string& path);
        bool saveJPG(const string& path, int quality);
        bool saveEXR(const string& filename);
    };
}

