// Internal Headers
#include "core/core.hpp"
#include "image_writer.hpp"
#include "graphics/raytracing_renderer.hpp"

// Macros
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__

// External Headers
#include "external/stb_image_write.hpp"

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
    default:
        format_str = ".error";
        break;
    }    

    // Create directory for images in case it does not already exist
    fs::create_directories(output_destination);

    // Calculate height given the width
    // height = int(width / aspect_ratio);
    // height = (height < 1) ? 1 : height;

    // Reserve space for image data pointer
    data.resize(4 * width * height);

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
    uint8_t red_byte, green_byte, blue_byte, alpha;
    std::tie<uint8_t, uint8_t, uint8_t, uint8_t>(red_byte, green_byte, blue_byte, alpha) = RGBA_color;

    // Write color info to image buffer
    data[pixel_position + 0] = red_byte;
    data[pixel_position + 1] = green_byte;
    data[pixel_position + 2] = blue_byte;
    data[pixel_position + 3] = alpha;
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
    }

    encoding_chrono.end();

    if (success)
        Logger::info("ImageWriter", "Image successfully written: " + full_name);
    else
        Logger::error("ImageWriter", "Failed to write image: " + full_name);

    size = file_size(get_file_size(image_path));
}

int Raytracing::ImageWriter::get_width()
{
    return width;
}

int Raytracing::ImageWriter::get_height()
{
    return height;
}

vector<uint8_t> Raytracing::ImageWriter::get_rgb_data()
{
    std::vector<unsigned char> rgb_data;
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

vector<uint8_t> Raytracing::ImageWriter::get_rgba_data()
{
    return data;
}

bool Raytracing::ImageWriter::savePNG(string path)
{
    int channels = 4; // RGBA
    int success = stbi_write_png(path.c_str(), width, height, channels, data.data(), width * channels);
    return success;
}

bool Raytracing::ImageWriter::saveJPG(string path, int quality)
{
    int channels = 3; // RGB
    auto data = get_rgb_data();
    int success = stbi_write_jpg(path.c_str(), width, height, channels, data.data(), quality);
    return success;
}
