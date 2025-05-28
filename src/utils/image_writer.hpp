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
    PNG_8,
    PNG_16,
    JPG,
    EXR_16,
    EXR_32
};

enum IMAGE_DYNAMIC_RANGE
{
    LDR,
    HDR
};

enum IMAGE_PIXEL_TYPE
{
    GRAYSCALE,          // 1 channel
    GRAYSCALE_ALPHA,    // 2 channels
    RGB,                // 3 channels
    RGBA,               // 4 channels
};

namespace Raytracing
{
    class ImageWriter
    {
    public:
        string name;                                // Image name
        string full_name;                           // Image name + extension
        IMAGE_FORMAT format = JPG;                  // Image format
        IMAGE_PIXEL_TYPE pixel_type = RGB;          // Image pixel type
        string format_str;                          // Image extension string
        string output_destination = output_path;    // Output path for the image
        int quality = 100;                          // Only for JPG images
        double aspect_ratio;                        // Ratio of image width over height
        file_size size;                             // Output image file size
        Chrono encoding_chrono;                     // Chrono to measure the image encoding time using stb_image_write

        ImageWriter();
        ImageWriter(int width, int height);

        void initialize();
        void initialize(const RendererSettings& settings);

        void write_pixel(const int pixel_row, const int pixel_column, const tuple<float, float, float, float> color_tuple);
        void save();

        int get_width() const;
        int get_height() const;

        int get_precision() const;

        vector<uint8_t> get_ubyte_data() const;
        vector<uint16_t> get_ushort_data() const;
        vector<float> get_float_data() const;
        vector<vector<float>> get_planar_data() const; // For TinyEXR support

        IMAGE_DYNAMIC_RANGE get_dynamic_range() const;
        int get_num_channels() const;

    private:

        vector<float> data;                     // Image buffer
        int precision = 8;                      // Bit precision of the output image

        int width;                              // Rendered image width in pixel count
        int height;                             // Rendered image height

        bool savePNG(const string& path);
        bool saveJPG(const string& path, int quality);
        bool saveEXR(const string& path);
        bool saveEXRcustom(const string& path);
    };
}

