#pragma once

// Headers
#include "core/core.hpp"
#include "math/matrix.hpp"

// Source: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/

class ToneMapper
{
public:
    static const Raytracing::Matrix33 ACESInputMat; // sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
    static const Raytracing::Matrix33 ACESOutputMat; // ODT_SAT => XYZ => D60_2_D65 => sRGB

    static vec3 RRTAndODTFit(vec3 v);

    // ACES fitted
    static Raytracing::color ACESFitted(Raytracing::color c);
    static tuple<double, double, double> ACESFitted(double r, double g, double b);

    // Reinhard
    static vec3 Reinhard(vec3 c);
};
