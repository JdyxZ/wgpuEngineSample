#pragma once

// Headers
#include "core/core.hpp"

// Forward declarations
struct file_size;
struct Chrono;

enum IMAGE_FORMAT
{
    PNG,
    JPG
};

namespace Raytracing
{
    class ImageWriter
    {
    public:
        string name;					        // The name is generated with timestamp
        string fmt_name;                        // The image name plus the image extension
        IMAGE_FORMAT format = JPG;              // Image format
        string format_str;                      // Image format string
        string image_path;                      // Short path based on cwd where image will be saved
        int precision = 8;                      // Bit precision of the output image
        int quality = 100;                      // Only for JPG images
        double aspect_ratio;                    // Ratio of image width over height
        shared_ptr<file_size> size;             // Output image file size
        shared_ptr<Chrono> encoding_chrono;     // Chrono to measure the image encoding time using stb_image_write

        ImageWriter();
        ImageWriter(int width, int height);

        void initialize();
        void write_pixel(const int pixel_row, const int pixel_column, const tuple<uint8_t, uint8_t, uint8_t, uint8_t> RGBA_color);
        void save();

        int get_width();
        int get_height();
        vector<uint8_t> get_rgb_data();
        vector<uint8_t> get_rgba_data();

    private:
        vector<uint8_t> data;                   // Image buffer (RGBA format)
        int width;                              // Rendered image width in pixel count
        int height;                             // Rendered image height

        bool savePNG();
        bool saveJPG(int quality);
    };
}

