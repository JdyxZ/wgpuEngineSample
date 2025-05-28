// Headers
#include "core/core.hpp"
#include "tone_mapper.hpp"

const Raytracing::Matrix33 ToneMapper::ACESInputMat = Raytracing::Matrix33
(
    0.59719, 0.35458, 0.04823,
    0.07600, 0.90834, 0.01566,
    0.02840, 0.13383, 0.83777
);

const Raytracing::Matrix33 ToneMapper::ACESOutputMat = Raytracing::Matrix33
(
    1.60475, -0.53108, -0.07367,
    -0.10208, 1.10813, -0.00605,
    -0.00327, -0.07276, 1.07602
);

vec3 ToneMapper::RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

Raytracing::color ToneMapper::ACESFitted(Raytracing::color _color)
{
    _color = ACESInputMat * _color;
    _color = RRTAndODTFit(_color); // Apply RRT and ODT
    _color = ACESOutputMat * _color;

    return _color;
}

tuple<double, double, double> ToneMapper::ACESFitted(double r, double g, double b)
{
    Raytracing::color _color (r, g, b);

    _color = ACESFitted(_color);

    return { _color.x, _color.y, _color.z };
}

vec3 ToneMapper::Reinhard(vec3 c)
{
    return c / (c + 1.0f);
}

vec3 ToneMapper::khronos_pbr_neutral(const vec3& color)
{
    vec3 final_color = color;

    double startCompression = 0.8 - 0.04;
    double desaturation = 0.15;

    double x = std::min(final_color.x, std::min(final_color.y, final_color.z));
    double offset = x < 0.08 ? (x - 6.25 * x * x) : .04;
    final_color -= offset;

    double peak = std::max(final_color.x, std::max(final_color.y, final_color.z));

    if (peak < startCompression) {
        return final_color;
    }

    double d = 1. - startCompression;
    double newPeak = 1. - d * d / (peak + d - startCompression);
    final_color *= newPeak / peak;

    double g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
    g = std::clamp(g, 0.0, 1.0);

    return final_color * (1.0 - g) + vec3(1.0, 1.0, 1.0) * g;
}

tuple<double, double, double> ToneMapper::khronos_pbr_neutral(double r, double g, double b)
{
    vec3 color = { r,g,b };

    vec3 final_color = khronos_pbr_neutral(color);

    return { final_color.x, final_color.y, final_color.z };
}
