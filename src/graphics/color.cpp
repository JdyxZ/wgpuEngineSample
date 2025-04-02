// Headers
#include "core/core.hpp"
#include "color.hpp"
#include "utils/interval.hpp"

// Usings
using Raytracing::color;

tuple<uint8_t, uint8_t, uint8_t> compute_color(const color& pixel_color)
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

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0.000, 0.999);
    int red_byte = static_cast<uint8_t>(256 * intensity.clamp(r));
    int green_byte = static_cast<uint8_t>(256 * intensity.clamp(g));
    int blue_byte = static_cast<uint8_t>(256 * intensity.clamp(b));

    return std::make_tuple(red_byte, green_byte, blue_byte);
}
