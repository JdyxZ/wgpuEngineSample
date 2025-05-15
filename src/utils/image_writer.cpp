// Internal Headers
#include "core/core.hpp"
#include "image_writer.hpp"
#include "graphics/raytracing_renderer.hpp"

// Macros
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__
#define TINYEXR_IMPLEMENTATION

// External Headers
#include "external/stb_image_write.hpp"
#include "external/tinyexr.h"

// Usings
using Raytracing::RendererSettings;

Raytracing::ImageWriter::ImageWriter() : width(0), height(0), aspect_ratio(0){}

Raytracing::ImageWriter::ImageWriter(int width, int height)
{
    this->width = width;
    this->height = height;
    this->aspect_ratio = width / static_cast<double>(height);
    this->encoding_chrono = Chrono();
}

void Raytracing::ImageWriter::initialize()
{
    // Set image extension
    switch (format)
    {
    case PNG:
        format_str = ".png";
        break;
    case JPG:
        format_str = ".jpg";
        break;
    case EXR:
        precision = 32;
        format_str = ".exr";
        break;
    default:
        format_str = ".error";
        break;
    }    

    // Create directory for images in case it does not already exist
    fs::create_directories(output_destination);

    // Reserve space for image data pointer
    auto dynamic_range = get_dynamic_range();
    switch (dynamic_range)
    {
    case IMAGE_DYNAMIC_RANGE::LDR:
        data.reserve(4 * width * height);
        break;
    case IMAGE_DYNAMIC_RANGE::HDR:
        r_data.reserve(width * height);
        g_data.reserve(width * height);
        b_data.reserve(width * height);
        // a_data.reserve(width * height);
        break;
    }

    // Log info
    Logger::info("ImageWriter", "Image frame succesfully initialized.");
}

void Raytracing::ImageWriter::initialize(RendererSettings& settings)
{
    format = settings.format;
    quality = settings.quality;
    output_destination = settings.image_path;

    initialize();
}

void Raytracing::ImageWriter::write_pixel(const int pixel_row, const int pixel_column, const tuple<uint8_t, uint8_t, uint8_t, uint8_t> RGBA_color)
{
    // Determine pixel position in image buffer
    int pixel_position = 4 * (width * pixel_row + pixel_column);

    // Unwrap color components
    auto [r, g, b, a] = RGBA_color;

    // Write color info to data buffer
    data[pixel_position + 0] = r;
    data[pixel_position + 1] = g;
    data[pixel_position + 2] = b;
    data[pixel_position + 3] = a;
}

void Raytracing::ImageWriter::write_pixel(const int pixel_row, const int pixel_column, const tuple<float, float, float, float> RGBA_color)
{
    // Determine pixel position in image buffer
    int pixel_position = 4 * (width * pixel_row + pixel_column);

    // Unwrap color components
    auto [r, g, b, a] = RGBA_color;

    // Write each color components in each color channel vector
    r_data[pixel_position] = r;
    g_data[pixel_position] = g;
    b_data[pixel_position] = b;
    // a_data[pixel_position] = a;
}

void Raytracing::ImageWriter::save()
{
    // Get current date and time
    name = get_current_timestamp();

    // Set formatted image name 
    full_name = name + format_str;

    // Set path for image saving
    string image_path = output_destination + "\\" + name + format_str;

    Logger::info("ImageWriter", "Encoding to image started.");

    bool success = false;

    encoding_chrono.start();

    switch (format)
    {
    case PNG:
        success = savePNG(image_path);
        break;
    case JPG:
        success = saveJPG(image_path, quality);
        break;
    case EXR:
        success = saveEXR(image_path);
        break;
    }

    encoding_chrono.end();

    if (success)
        Logger::info("ImageWriter", "Image successfully written: " + full_name);
    else
        Logger::error("ImageWriter", "Failed to write image: " + full_name);

    size = file_size(get_file_size(image_path));
}

int Raytracing::ImageWriter::get_width() const
{
    return width;
}

int Raytracing::ImageWriter::get_height() const
{
    return height;
}

vector<uint8_t> Raytracing::ImageWriter::get_rgb_data() const
{
    if (get_dynamic_range() == IMAGE_DYNAMIC_RANGE::HDR)
    {
        string error = Logger::error("ImageWriter", "Cannot use the rgb data function with HDR image output");
        throw std::runtime_error(error);
    }

    vector<uint8_t> rgb_data;
    rgb_data.reserve(width * height * 3);

    for (size_t i = 0; i < data.size(); i += 4)
    {
        rgb_data.push_back(data[i]);        // R
        rgb_data.push_back(data[i + 1]);    // G
        rgb_data.push_back(data[i + 2]);    // B
                                            // Skip A
    }

    return rgb_data;
}

vector<uint8_t> Raytracing::ImageWriter::get_rgba_data() const
{
    if (get_dynamic_range() == IMAGE_DYNAMIC_RANGE::HDR)
    {
        string error = Logger::error("ImageWriter", "Cannot use the rgba data function with HDR image output");
        throw std::runtime_error(error);
    }

    return data;
}

IMAGE_DYNAMIC_RANGE Raytracing::ImageWriter::get_dynamic_range() const
{
    precision == 8 ? IMAGE_DYNAMIC_RANGE::LDR : IMAGE_DYNAMIC_RANGE::HDR;
}

bool Raytracing::ImageWriter::savePNG(const string& path)
{
    int channels = 4; // RGBA
    int success = stbi_write_png(path.c_str(), width, height, channels, data.data(), width * channels);
    return success;
}

bool Raytracing::ImageWriter::saveJPG(const string& path, int quality)
{
    int channels = 3; // RGB
    auto data = get_rgb_data();
    int success = stbi_write_jpg(path.c_str(), width, height, channels, data.data(), quality);
    return success;
}

bool Raytracing::ImageWriter::saveEXR(const string& path)
{
    // Define number of channels
    int num_channels = 3;

    // Assuming you have alpha data stored in a vector<float> a_data;
    float* images_RGB[4] = { r_data.data(), g_data.data(), b_data.data() };
    float* images_RGBA[4] = { r_data.data(), g_data.data(), b_data.data(), a_data.data() };

    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage exr_image;
    InitEXRImage(&exr_image);

    exr_image.num_channels = num_channels;
    exr_image.images = num_channels == 3 ? (unsigned char**)images_RGB : (unsigned char**)images_RGBA; // Cast array of float* to unsigned char**
    exr_image.width = width;
    exr_image.height = height;

    // Channel names in B, G, R, A order (important)
    header.num_channels = num_channels;
    header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
    strncpy(header.channels[0].name, "B", 255);
    strncpy(header.channels[1].name, "G", 255);
    strncpy(header.channels[2].name, "R", 255);
    if(num_channels == 4) strncpy(header.channels[3].name, "A", 255);

    // Pixel types arrays (input and output types)
    header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
    header.requested_pixel_types = (int*)malloc(sizeof(int) * header.num_channels);

    for (int i = 0; i < num_channels; ++i)
    {
        header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;        // input pixels are floats
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // store as float in EXR
    }

    // Set compression method here (e.g., ZIP_COMPRESSION)
    // header.compression_type = TINYEXR_COMPRESSION_ZIP; 

    const char* err = nullptr;
    int result = SaveEXRImageToFile(&exr_image, &header, path.c_str(), &err);

    free(header.channels);
    free(header.pixel_types);
    free(header.requested_pixel_types);

    if (result != TINYEXR_SUCCESS) {
        if (err) {
            fprintf(stderr, "TinyEXR Error: %s\n", err);
            FreeEXRErrorMessage(err);
        }
        return false;
    }

    return true;
}

