// Headers
#include "core/core.hpp"
#include "color.hpp"
#include "math/interval.hpp"

// Usings
using Raytracing::color;

tuple<uint8_t, uint8_t, uint8_t, uint8_t> compute_color(const color& pixel_color)
{
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    // Replace NaN components with zero to kill shadow acne
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Apply tone mapper (for HDR textures) or final image is HDR 


    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0.000, 0.999);
    uint8_t red_byte = static_cast<uint8_t>(256 * intensity.clamp(r));
    uint8_t green_byte = static_cast<uint8_t>(256 * intensity.clamp(g));
    uint8_t blue_byte = static_cast<uint8_t>(256 * intensity.clamp(b));
    uint8_t alpha = 1.0;

    return std::make_tuple(red_byte, green_byte, blue_byte, alpha);
}
