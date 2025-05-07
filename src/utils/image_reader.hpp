#pragma once

// Headers
#include "core/core.hpp"
#include "graphics/texture.h"

namespace Raytracing
{
    class ImageReader
    {
    public:

        // Loads image data from the specified file. If the textures_directory environment variable is
        // defined, looks only in that directory for the image file. If the image was not found,
        // searches for the specified image file first from the current directory, then in the
        // {textures_path}/ subdirectory, then the _parent's_ images/ subdirectory, and then _that_
        // parent, on so on, for six levels up. If the image was not loaded successfully,
        // width() and height() will return 0.
        ImageReader(const char* image_filename);

        ImageReader();
        ImageReader(const sTextureData& tex_data);
        ~ImageReader();

        int width()  const;
        int height() const;

        // Loads the linear (gamma=1) image data from the given file name. Returns true if the
        // load succeeded. The resulting data buffer points to the three RGB [0.0, 1.0]
        // floating-point values of the first pixel (red, then green, then blue). Pixels are
        // contiguous, going left to right for the width of the image, followed by the next row
        // below, for the full height of the image.
        bool load(const string& filename);
        const uint8_t* pixel_data(int x, int y) const;

    private:
        int             bytes_per_pixel;
        float*          fdata = nullptr;            // Linear floating point pixel data
        const uint8_t*  bdata = nullptr;            // Linear 8-bit pixel data
        int             image_width = 0;            // Loaded image width
        int             image_height = 0;           // Loaded image height
        int             bytes_per_scanline = 0;

        static unsigned char float_to_byte(float value);
        void convert_to_bytes(); // Converts the linear floating point pixel data to bytes, storing the resulting byte data in the `bdata` member.
    };
}
