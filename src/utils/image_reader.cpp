// Internal Headers
#include "core/core.hpp"
#include "image_reader.hpp"
#include "utils/utilities.hpp"

// External Headers
#include "stb_image.h"

Raytracing::ImageReader::ImageReader(const char* image_filename)
{
    auto filename = string(image_filename);

    // Hunt for the image file in some likely locations.
    string prefix = "";
    for (int i = 0; i < 7; ++i)
    {
        if (load(prefix + textures_directory + "\\" + filename)) return;
        prefix += "..\\";
    }
    if (load(filename)) return;

    Logger::error("ImageReader", "Could not load image file: " + string(image_filename));
}

Raytracing::ImageReader::ImageReader() {}

Raytracing::ImageReader::ImageReader(const sTextureData& tex_data)
{
    // Set bdata to point to texture data
    bdata = tex_data.data.data();

    // Set image specs
    image_width = tex_data.image_width;
    image_height = tex_data.image_height;
    bytes_per_pixel = tex_data.bytes_per_pixel;
    bytes_per_scanline = image_width * bytes_per_pixel;
}

Raytracing::ImageReader::~ImageReader()
{
    // delete[] bdata; ERROR WITH ENGINE TEXTURES!!!
    free(fdata);
}

int Raytracing::ImageReader::width()  const
{ 
    return (bdata == nullptr) ? 0 : image_width; 
}

int Raytracing::ImageReader::height() const
{ 
    return (bdata == nullptr) ? 0 : image_height; 
}

bool Raytracing::ImageReader::load(const std::string& filename)
{
    // Loads the linear (gamma=1) image data from the given file name. Returns true if the
    // load succeeded. The resulting data buffer points to the three RGB [0.0, 1.0]
    // floating-point values of the first pixel (red, then green, then blue). Pixels are
    // contiguous, going left to right for the width of the image, followed by the next row
    // below, for the full height of the image.

    bytes_per_pixel = 3;
    auto n = bytes_per_pixel; // Dummy out parameter: original components per pixel
    fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);

    if (fdata == nullptr) return false;

    bytes_per_scanline = image_width * bytes_per_pixel;
    convert_to_bytes();

    return true;
}

const uint8_t* Raytracing::ImageReader::pixel_data(int x, int y) const
{
    // Return the address of the three RGB bytes of the pixel at x,y. If there is no image
    // data, returns magenta.
    static uint8_t magenta[] = { 255, 0, 255 };
    if (bdata == nullptr) return magenta;

    if (!is_within(x, 0, image_width, BoundType::inclusive, BoundType::exclusive))
    {
        string error = Logger::error("IMAGE_READER", std::format("Pixel horizontal location {} is outside of texture width bound [0, {})", x, image_width));
        throw std::invalid_argument(error);
    }

    if (!is_within(y, 0, image_height, BoundType::inclusive, BoundType::exclusive))
    {
        string error = Logger::error("IMAGE_READER", std::format("Pixel vertial location {} is outside of texture height bound [0, {})", y, image_height));
        throw std::invalid_argument(error);
    }

    return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
}

unsigned char Raytracing::ImageReader::float_to_byte(float value)
{
    if (value <= 0.0)
        return 0;
    if (1.0 <= value)
        return 255;
    return static_cast<unsigned char>(256.0 * value);
}

void Raytracing::ImageReader::convert_to_bytes()
{
    // Convert the linear floating point pixel data to bytes, storing the resulting byte
    // data in the `bdata` member.
    int total_bytes = image_width * image_height * bytes_per_pixel;

    // Iterate through all pixel components, converting from [0.0, 1.0] float values to
    // unsigned [0, 255] byte values.
    auto* bptr = new uint8_t[total_bytes];
    auto* fptr = fdata;
    for (auto i = 0; i < total_bytes; i++, fptr++, bptr++)
        *bptr = float_to_byte(*fptr);

    bdata = bptr;
}

