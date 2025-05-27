// Internal Headers
#include "core/core.hpp"
#include "image_writer.hpp"
#include "graphics/raytracing_renderer.hpp"

// Macros
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__
#define TINYEXR_IMPLEMENTATION
#define TINYEXR_USE_MINIZ 0

// External Headers
#include "external/stb_image_write.hpp"
#include "zlib.h"
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
    // Validate image attributes
    switch (format)
    {
    case JPG:
        if (pixel_type == GRAYSCALE_ALPHA)
        {
            Logger::warn("ImageWriter", "JPG format does not support alpha channel. Using GRAYSCALE instead.");
            pixel_type = GRAYSCALE;
        }
        if (pixel_type == RGBA)
        {
            Logger::warn("ImageWriter", "JPG format does not support alpha channel. Using RGB instead.");
            pixel_type = RGB; 
        }

        quality = std::clamp(quality, 1, 100);
        break;

    case EXR_16:
    case EXR_32:

        if (pixel_type == GRAYSCALE_ALPHA)
        {
            Logger::warn("ImageWriter", "EXR format does not support grayscale alpha pixel types. Using Grayscale instead");
            pixel_type = GRAYSCALE;
        }
        break;
    }

    // Set image extension
    switch (format)
    {
    case PNG_8:
        format_str = ".png";
        precision = 8;
        break;
    case PNG_16:
        format_str = ".png"; 
        precision = 16;       
        break;
    case JPG:
        precision = 8; 
        format_str = ".jpg";
        break;
    case EXR_16:
        precision = 16;
        format_str = ".exr";
        break;
    case EXR_32:
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
    int num_channels = get_num_channels();
    data.resize(num_channels * width * height);

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

void Raytracing::ImageWriter::write_pixel(const int pixel_row, const int pixel_column, const tuple<float, float, float, float> color_tuple)
{
    // Determine pixel position in image buffer
    int num_channels = get_num_channels();
    int pixel_position = num_channels * (width * pixel_row + pixel_column);

    // Unwrap color components
    auto [r, g, b, a] = color_tuple;

    // Write color info to data buffer
    switch (pixel_type)
    {
    case GRAYSCALE:
        data[pixel_position + 0] = r;  // Three first channels are the same (r = g = b), so we can take r as the grayscale value
        break;
    case GRAYSCALE_ALPHA:
        data[pixel_position + 0] = r;  // Three first channels are the same (r = g = b), so we can take r as the grayscale value
        data[pixel_position + 1] = a;  
        break;
    case RGB:
        data[pixel_position + 0] = r;
        data[pixel_position + 1] = g;
        data[pixel_position + 2] = b;
        break;
    case RGBA:
        data[pixel_position + 0] = r;
        data[pixel_position + 1] = g;
        data[pixel_position + 2] = b;
        data[pixel_position + 3] = a;
        break;
    }
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
    case PNG_8:
    case PNG_16:
        success = savePNG(image_path);
        break;
    case JPG:
        success = saveJPG(image_path, quality);
        break;
    case EXR_16:
    case EXR_32:
        success = saveEXRcustom(image_path);
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

int Raytracing::ImageWriter::get_precision() const
{
    return precision;
}

vector<uint8_t> Raytracing::ImageWriter::get_ubyte_data() const
{
    vector<uint8_t> ubyte_data(data.size());

    for (size_t i = 0; i < data.size(); ++i)
        ubyte_data[i] = static_cast<uint8_t>(std::clamp(data[i], 0.0f, 1.0f) * 255.0f);

    return ubyte_data;
}

vector<uint16_t> Raytracing::ImageWriter::get_ushort_data() const
{
    vector<uint16_t> ushort_data(data.size());

    for (size_t i = 0; i < data.size(); ++i)
        ushort_data[i] = static_cast<uint16_t>(std::clamp(data[i], 0.0f, 1.0f) * 65535.0f);

    return ushort_data;
}

vector<float> Raytracing::ImageWriter::get_float_data() const
{
    return data;
}

vector<vector<float>> Raytracing::ImageWriter::get_planar_data() const
{
    int num_channels = get_num_channels();
    size_t num_pixels = data.size() / num_channels; // This is equivalent to width * height

    vector<vector<float>> planar_data(num_channels);

    // Reserve space
    for (size_t i = 0; i < num_channels; ++i)
        planar_data[i].reserve(num_pixels);

    // Fill planar data
    for (size_t i = 0; i < data.size(); i++)
        planar_data[i % num_channels].push_back(data[i]);

    return planar_data;
}

IMAGE_DYNAMIC_RANGE Raytracing::ImageWriter::get_dynamic_range() const
{
    return format == EXR_16 || format == EXR_32 ? IMAGE_DYNAMIC_RANGE::HDR : IMAGE_DYNAMIC_RANGE::LDR;
}

int Raytracing::ImageWriter::get_num_channels() const
{
    switch (pixel_type)
    {
    case GRAYSCALE:
        return 1;
    case GRAYSCALE_ALPHA:
        return 2;
    case RGB:
        return 3;
    case RGBA:
        return 4;
    }

    return 0;
}

bool Raytracing::ImageWriter::savePNG(const string& path)
{
    bool success = false;
    int channels = get_num_channels();

    if (format == PNG_8)
    {
        auto ubyte_data = get_ubyte_data();
        success = stbi_write_png(path.c_str(), width, height, channels, ubyte_data.data(), width * channels * sizeof(uint8_t));
    }
    else if (format == PNG_16)
    {
        auto ushort_data = get_ushort_data();
        success = stbi_write_png(path.c_str(), width, height, channels, ushort_data.data(), width * channels * sizeof(uint16_t));
    }

    return success;
}

bool Raytracing::ImageWriter::saveJPG(const string& path, int quality)
{
    int channels = get_num_channels();
    auto ubyte_data = get_ubyte_data();

    bool success = stbi_write_jpg(path.c_str(), width, height, channels, ubyte_data.data(), quality);
    return success;
}

bool Raytracing::ImageWriter::saveEXR(const string& path)
{
    int num_channels = get_num_channels();
    bool save_as_fp16 = (format == EXR_16) ? 1 : 0;

    const char** err = nullptr;
    bool success = ::SaveEXR(data.data(), width, height, num_channels, save_as_fp16, path.c_str(), err);

    if (success != TINYEXR_SUCCESS)
    {
        if (err)
        {
            Logger::error("ImageWriter", "TinyEXR Error: " + string(*err));
            FreeEXRErrorMessage(*err);
        }

        return false;
    }

    return true;
}

bool Raytracing::ImageWriter::saveEXRcustom(const string& path)
{  
    // Get number of channels
    int num_channels = get_num_channels();

    // Get image planar data
    auto planar_data = get_planar_data();

    // The order of the channels must be (A)BGR since most of EXR viewers expect this channel order.
    std::reverse(planar_data.begin(), planar_data.end());

    // Convert to float pointer array
    vector<float*> image_ptr(num_channels); 
    for (int i = 0; i < num_channels; ++i)
        image_ptr[i] = planar_data[i].data();

    EXRHeader header; InitEXRHeader(&header);
    EXRImage exr_image; InitEXRImage(&exr_image);

    exr_image.num_channels = num_channels;
    exr_image.images = (unsigned char**)image_ptr.data();
    exr_image.width = width;
    exr_image.height = height;

    // Allocate space for each channel
    vector<EXRChannelInfo> channels(num_channels);
    header.channels = channels.data();
    header.num_channels = num_channels;

    switch (pixel_type)
    {
    case GRAYSCALE:
        strncpy_s(header.channels[0].name, "Y", 255); 
        break;
    case GRAYSCALE_ALPHA:
        return false;
    case RGB:
        strncpy_s(header.channels[0].name, "B", 255); 
        strncpy_s(header.channels[1].name, "G", 255); 
        strncpy_s(header.channels[2].name, "R", 255); 
        break;
    case RGBA:
        strncpy_s(header.channels[0].name, "A", 255); 
        strncpy_s(header.channels[1].name, "B", 255);
        strncpy_s(header.channels[2].name, "G", 255); 
        strncpy_s(header.channels[3].name, "R", 255); 
        break;
    }

    // Define input/ouput pixel bit depth
    vector<int> pixel_types(num_channels, TINYEXR_PIXELTYPE_FLOAT);
    vector<int> requested_pixel_types(num_channels, format == EXR_16 ? TINYEXR_PIXELTYPE_HALF : TINYEXR_PIXELTYPE_FLOAT);
    header.pixel_types = pixel_types.data();
    header.requested_pixel_types = requested_pixel_types.data();

    // Set compression method
    // header.compression_type = TINYEXR_COMPRESSIONTYPE_ZIP;
    header.compression_type = TINYEXR_COMPRESSIONTYPE_PIZ;

    const char* err = nullptr; 
    int success = SaveEXRImageToFile(&exr_image, &header, path.c_str(), &err);

    if (success != TINYEXR_SUCCESS)
    {
        if (err)
        {
            Logger::error("ImageWriter", "TinyEXR Error: " + string(err));
            FreeEXRErrorMessage(err);
        }

        return false;
    }

    return true;
}

