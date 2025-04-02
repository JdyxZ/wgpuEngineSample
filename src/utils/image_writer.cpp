// Internal Headers
#include "core/core.hpp"
#include "image_writer.hpp"
#include "utilities.hpp"
#include "chrono.hpp"

// Macros
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__

// External Headers
#include "external/stb_image_write.hpp"

Raytracing::ImageWriter::ImageWriter() {}

Raytracing::ImageWriter::ImageWriter(int width, int height)
{
    this->width = width;
    this->height = height;
    this->aspect_ratio = width / (double)height;
    encoding_chrono = make_shared<Chrono>();
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

    // Construct the file name and path
    string folderPath = output_directory + "\\";

    // Create directory for images in case it does not already exist
    fs::create_directories(folderPath);

    // Calculate height given the width
    // height = int(width / aspect_ratio);
    // height = (height < 1) ? 1 : height;

    // Reserve space for image data pointer
    data.resize(3 * width * height);

    // Log info
    Logger::info("ImageWriter", "Image frame succesfully initialized.");
}

void Raytracing::ImageWriter::write_pixel(int pixel_position, tuple<uint8_t, uint8_t, uint8_t> RGB_color)
{
    uint8_t red_byte, green_byte, blue_byte;
    std::tie<uint8_t, uint8_t, uint8_t>(red_byte, green_byte, blue_byte) = RGB_color;

    data[pixel_position + 0] = red_byte;
    data[pixel_position + 1] = green_byte;
    data[pixel_position + 2] = blue_byte;
}

void Raytracing::ImageWriter::save()
{
    // Get current date and time
    name = get_current_timestamp();

    // Set formatted image name 
    fmt_name = name + format_str;

    // Set path for image saving
    image_path = output_path + "\\" + name + format_str;

    Logger::info("ImageWriter", "Encoding to image started.");

    bool success = false;

    encoding_chrono->start();

    switch (format)
    {
    case PNG:
        success = savePNG();
        break;
    case JPG:
        success = saveJPG(quality);
        break;
    }

    encoding_chrono->end();

    if (success)
        Logger::info("ImageWriter", "Image successfully written: " + fmt_name);
    else
        Logger::error("ImageWriter", "Failed to write image: " + fmt_name);

    size = make_shared<file_size>(get_file_size(image_path));
}

int Raytracing::ImageWriter::get_width()
{
    return width;
}

int Raytracing::ImageWriter::get_height()
{
    return height;
}

vector<uint8_t> Raytracing::ImageWriter::get_data()
{
    return data;
}

bool Raytracing::ImageWriter::savePNG()
{
    int success = stbi_write_png(image_path.c_str(), width, height, channels, data.data(), width * channels);
    return success;
}

bool Raytracing::ImageWriter::saveJPG(int quality)
{
    int success = stbi_write_jpg(image_path.c_str(), width, height, channels, data.data(), quality);
    return success;
}
