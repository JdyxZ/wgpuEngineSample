// Headers
#include "core/core.hpp"
#include "color.hpp"
#include "math/interval.hpp"
#include "graphics/tone_mapper.hpp"
#include "utils/image_writer.hpp"

// Usings
using Raytracing::color;

tuple<float, float, float, float> compute_LDR_color(const color& pixel_color)
{
    // Unwrap the color components
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;
    float a = 1;

    // Replace NaN components with zero to kill shadow acne
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Apply tone mapper to map HDR to LDR
    auto [rc, gc, bc] = ToneMapper::ACESFitted(r, g, b);

    // Clamp negative values to 0 to avoid NaNs when applying linear to gamma conversion
    r = std::max(0.0, rc);
    g = std::max(0.0, gc);
    b = std::max(0.0, bc);

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Return mapped, converted and clamped colors
    return { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), a };
}

tuple<float, float, float, float> compute_HDR_color(const color& pixel_color)
{
    // Unwrap the color components
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;
    float a = 1;

    // Replace NaN components with zero to kill shadow acne
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Since output image is in HDR format, no need to apply transformations to color
    return { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), a};
}

tuple<float, float, float, float> compute_color(const color& pixel_color, IMAGE_DYNAMIC_RANGE dynamic_range)
{
    switch (dynamic_range)
    {
    case(IMAGE_DYNAMIC_RANGE::LDR):
        return compute_LDR_color(pixel_color);
    case(IMAGE_DYNAMIC_RANGE::HDR):
        return compute_HDR_color(pixel_color);
    }

    return { static_cast<float>(MAGENTA.x), static_cast<float>(MAGENTA.y), static_cast<float>(MAGENTA.z), 1.0f }; // Fallback color in case of error
}
