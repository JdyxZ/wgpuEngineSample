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
        IMAGE_FORMAT format = PNG;              // Image format
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
        void write_pixel(int pixel_position, tuple<uint8_t, uint8_t, uint8_t> RGB_color);
        void save();

        int get_width();
        int get_height();
        vector<uint8_t> get_data();

    private:
        vector<uint8_t> data;             // Image buffer
        int channels = 3;                       // RGB channels
        int width;                              // Rendered image width in pixel count
        int height;                             // Rendered image height

        bool savePNG();
        bool saveJPG(int quality);
    };
}

