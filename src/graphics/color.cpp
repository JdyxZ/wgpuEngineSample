// Headers
#include "core/core.hpp"
#include "color.hpp"
#include "math/interval.hpp"
#include "graphics/tone_mapper.hpp"
#include "utils/image_writer.hpp"

// Usings
using Raytracing::color;

tuple<uint8_t, uint8_t, uint8_t, uint8_t> compute_LDR_color(const color& pixel_color)
{
    // Unwrap the color components
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    // Replace NaN components with zero to kill shadow acne
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Apply tone mapper to map HDR to LDR
    auto [r, g, b] = ToneMapper::ACESFitted(r, g, b);

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0.000, 0.999);
    uint8_t red_byte = static_cast<uint8_t>(256 * intensity.clamp(r));
    uint8_t green_byte = static_cast<uint8_t>(256 * intensity.clamp(g));
    uint8_t blue_byte = static_cast<uint8_t>(256 * intensity.clamp(b));
    uint8_t alpha = 1;

    return { red_byte, green_byte, blue_byte, alpha };
}

tuple<float, float, float, float> compute_HDR_color(const color& pixel_color)
{
    // Unwrap the color components
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;
    double a = 1;

    // Replace NaN components with zero to kill shadow acne
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Since output image is in HDR format, no need to apply transformations to color
    return { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a) };
}
