#pragma once

// Headers
#include "math/vec3.hpp"
#include "core/core.hpp"

// Colors
const Raytracing::color RED(1.0, 0.0, 0.0);
const Raytracing::color GREEN(0.0, 1.0, 0.0);
const Raytracing::color BLUE(0.0, 0.0, 1.0);
const Raytracing::color CYAN(0.0, 1.0, 1.0);
const Raytracing::color MAGENTA(1.0, 0.0, 1.0);
const Raytracing::color YELLOW(1.0, 1.0, 0.0);
const Raytracing::color WHITE(1.0, 1.0, 1.0);
const Raytracing::color BLACK(0.0, 0.0, 0.0);
const Raytracing::color BROWN(0.6, 0.3, 0.0);
const Raytracing::color PINK(1.0, 0.75, 0.8);
const Raytracing::color PURPLE(0.5, 0.0, 0.5);
const Raytracing::color GRAY(0.5, 0.5, 0.5);
const Raytracing::color ORANGE(1.0, 0.65, 0.0);
const Raytracing::color SKY_BLUE(135.0 / 255.0, 206.0 / 255.0, 235.0 / 255.0);

inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

tuple<uint8_t, uint8_t, uint8_t> compute_color(const Raytracing::color& pixel_color);


