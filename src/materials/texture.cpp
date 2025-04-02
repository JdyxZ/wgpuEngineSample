// Headers
#include "core/core.hpp"
#include "texture.hpp"
#include "math/perlin.hpp"
#include "utils/image_reader.hpp"
#include "graphics/color.hpp"
#include "utils/interval.hpp"

// Usings
using Raytracing::color;
using Raytracing::ImageReader;

Raytracing::SolidColor::SolidColor(const color& albedo) : albedo(albedo) {}

Raytracing::SolidColor::SolidColor(double red, double green, double blue) : SolidColor(color(red, green, blue)) {}

color Raytracing::SolidColor::value(pair<double, double> texture_coordinates, const point3& p) const
{
    return albedo;
}

Raytracing::CheckerTexture::CheckerTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
    : inv_scale(1.0 / scale), even(even), odd(odd) {}

Raytracing::CheckerTexture::CheckerTexture(double scale, const color& c1, const color& c2)
    : CheckerTexture(scale, make_shared<SolidColor>(c1), make_shared<SolidColor>(c2)) {}

color Raytracing::CheckerTexture::value(pair<double, double> texture_coordinates, const point3& p) const
{
    auto x = int(floor(inv_scale * p.x));
    auto y = int(floor(inv_scale * p.y));
    auto z = int(floor(inv_scale * p.z));

    bool isEven = (x + y + z) % 2 == 0;

    return isEven ? even->value(texture_coordinates, p) : odd->value(texture_coordinates, p);
}

Raytracing::ImageTexture::ImageTexture(const char* filename)
{
    image = make_shared<ImageReader>(filename);
}

Raytracing::ImageTexture::ImageTexture(string filename)
{
    image = make_shared<ImageReader>(filename.c_str());
}

color Raytracing::ImageTexture::value(pair<double, double> texture_coordinates, const point3& p) const
{
    // Unwrap texture coordinates
    auto [u, v] = texture_coordinates;

    // If we have no texture data, then return solid cyan as a debugging aid.
    if (image->height() <= 0) return CYAN;

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = Interval::unitary.clamp(u);
    v = 1.0 - Interval::unitary.clamp(v);  // Flip V to image coordinates

    // Get pixel location
    auto i = int(u * image->width());
    auto j = int(v * image->height());

    // Read pixel data
    auto pixel = image->pixel_data(i, j);

    // Scale pixel color to 8 bit data
    auto color_scale = 1.0 / 255.0;
    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}

Raytracing::NoiseTexture::NoiseTexture(double scale, int depth) : scale(scale), depth(depth)
{
    noise = make_shared<Perlin>();
}

color Raytracing::NoiseTexture::value(pair<double, double> texture_coordinates, const point3& p) const
{
    return color(.5, .5, .5) * (1 + std::sin(scale * p.z + 10 * noise->turbulance(p, depth)));
}
