#pragma once

// Headers
#include "core/core.hpp"
#include "graphics/texture.h"
#include "math/vec3.hpp"

namespace Raytracing
{
    enum class ImageDataType
    {
        UINT8_T,
        UINT16_T,
        FLOAT
    };

    enum class ImageType
    {
        PARSED, // Image was parsed from the webgpu engine framework
        LOADED // Image was loaded from a file
    };

    struct ImageReader
    {
    public:

        // Loads image data from the specified file. If the textures_directory environment variable is
        // defined, looks only in that directory for the image file. If the image was not found,
        // searches for the specified image file first from the current directory, then in the
        // {textures_path}/ subdirectory, then the _parent's_ images/ subdirectory, and then _that_
        // parent, on so on, for six levels up. If the image was not loaded successfully,
        // width() and height() will return 0.
        ImageReader(const char* image_filename); // Loading constructor
        ImageReader(const sTextureData& tex_data); // Parsing constructor

        ~ImageReader();

        int width()  const;
        int height() const;

        // Returns true if the load succeeded.
        // Loads the linear (gamma=1) image data from the given file name.
        // The resulting data buffer points to the channel values of the first pixel.
        // Pixels are contiguous, going left to right for the width of the image,
        // followed by the next row below, for the full height of the image.
        bool load(const string& filename);

        // Reads texture data and returns the address of the three RGB bytes [0.0, 1.0] of the pixel at x,y.
        // If there is no image data, returns magenta.
        const color pixel_data(int x, int y) const;

        static uint8_t* convert_float_to_uint8(const float* float_data, size_t count);
        static uint16_t* convert_float_to_uint16(const float* float_data, size_t count);

    private:
        bool            is_linear;                          // Is the image linear (gamma = 1) or not (gamma != 1)
        int             channels;                           // Number of channels (1, 3, 4, etc.)
        int             bit_depth;                          // Number of bits per channel (8 bits, 16 bits, 32 bits, etc.)
        ImageType       image_type;                         // Type of the image (parsed or loaded)
        ImageDataType   data_type;                          // Data type of the image data based on the bit depth
        int             image_width = 0;                    // Loaded image width
        int             image_height = 0;                   // Loaded image height
        int             bytes_per_pixel = 0;                // Number of bytes per pixel (num_channels x bit_depth) 
        int             bytes_per_scanline = 0;             // Number of bytes per scanline (width x bytes_per_pixel)

        // Owned data (loaded from file)
        uint8_t* owned_uint8_data = nullptr;                // Linear 8-bit pixel data
        uint16_t* owned_uint16_data = nullptr;              // Linear 16-bit pixel data
        float* owned_float_data = nullptr;                  // Linear 32-bit pixel data

        // Parsed data (from the webgpu engine framework)
        const uint8_t* external_uint8_data = nullptr;       // Linear 8-bit pixel data
        const uint16_t* external_uint16_data = nullptr;     // Linear 16-bit pixel data
        const float* external_float_data = nullptr;         // Linear 32-bit pixel data

        ImageDataType get_data_type(int bit_depth) const;
        Raytracing::ImageDataType get_data_type(const string& filename) const;
    };
}
