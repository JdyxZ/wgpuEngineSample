#pragma once

// Headers
#include "core/core.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"

// ************** TIMESTAMP UTILITIES ************** //

bool is_valid_timestamp_format(const char* fmt);
string get_current_timestamp(const char* fmt = "%a %b %d %H_%M_%S %Y", bool miliseconds = false);

// ************** FILE UTILITIES ************** //

struct file_size
{
    double amount;
    string unit;
};

file_size get_file_size(const string& file_path);

// ************** STRING UTILITIES ************** //

string to_list(const vector<string>& vec);
string trim(const string& str);
string trim_trailing_zeros(const double number, const bool remove_point = true);

// ************** MATH UTILITIES ************** //

constexpr double clamp(double value, double min, double max) 
{
    if (min > max) std::swap(min, max);
    return std::max(min, std::min(value, max));
}

constexpr double degrees_to_radians(double degrees) 
{ 
    return degrees * Raytracing::pi / 180.0; 
}

inline double random_double() // Returns a random real in [0,1).
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) // Returns a random real in [min,max).
{
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) // Returns a random integer in [min,max].
{
    return int(random_double(min, max + 1));
}

// ************** VECTOR UTILITIES ************** //

inline double dot(const vec3& u, const vec3& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline double dot(const vec4& u, const vec4& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z + u.w * u.z;
}

inline vec3 cross(const vec3& u, const vec3& v)
{
    return vec3
    (
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    );
}

inline vec3 unit_vector(const vec3& v)
{
    return v / v.length();
}

inline vec4 unit_vector(const vec4& v)
{
    return v / v.length();
}

inline vec3 reflect(const vec3& v, const vec3& n)
{
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& incoming_ray, const vec3& n, double cos_theta, double refraction_index)
{
    vec3 refracted_ray_perpendicular = refraction_index * (incoming_ray + cos_theta * n);
    vec3 refracted_ray_parellel = -std::sqrt(std::fabs(1.0 - refracted_ray_perpendicular.length_squared())) * n;
    return refracted_ray_perpendicular + refracted_ray_parellel;
}

inline vec3 lerp(double a, vec3 start, vec3 end)
{
    return (1.0 - a) * start + a * end;
}

inline vec3 min_vector(const vec3& v1, const vec3& v2)
{
    return vec3(std::fmin(v1.x, v2.x), std::fmin(v1.y, v2.y), std::fmin(v1.z, v2.z));
}

inline vec3 max_vector(const vec3& v1, const vec3& v2)
{
    return vec3(std::fmax(v1.x, v2.x), std::fmax(v1.y, v2.y), std::fmax(v1.z, v2.z));
}

// ************** SAMPLE UTILITIES ************** //

inline vec3 sample_square() // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
{
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

inline vec3 sample_square_stratified(int sample_row, int sample_column, double pixel_sample_sqrt_inv) // Returns the vector to a random point in the square sub-pixel specified by grid indices sample_row and sample_column, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].
{
    auto px = ((sample_column + random_double()) * pixel_sample_sqrt_inv) - 0.5;
    auto py = ((sample_row + random_double()) * pixel_sample_sqrt_inv) - 0.5;

    return vec3(px, py, 0);
}

inline vec3 random_in_unit_disk() // Returns a random point in the unit disk.
{
    while (true)
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 sample_disk(double radius) // Returns a random point in the disk centered at origin.
{
    return radius * random_in_unit_disk();
}

inline point3 defocus_disk_sample(vec3 center, vec3 defocus_disk_u, vec3 defocus_disk_v) // Returns a random point in the defocus disk given.
{
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

inline vec3 random_unit_vector()
{
    while (true)
    {
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();
        if (practically_zero < lensq && lensq <= 1.0)
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal)
{
    vec3 unit_vector_on_sphere = random_unit_vector();

    // If the vector is in the same hemisphere than the normal return it, otherwise return its opposite.
    return (dot(unit_vector_on_sphere, normal) > 0.0) ? unit_vector_on_sphere : -unit_vector_on_sphere;
}

inline vec3 random_cosine_hemisphere_direction()
{
    auto r1 = random_double();
    auto r2 = random_double();

    auto phi = 2 * Raytracing::pi * r1;
    auto x = std::cos(phi) * std::sqrt(r2);
    auto y = std::sin(phi) * std::sqrt(r2);
    auto z = std::sqrt(1 - r2);

    return vec3(x, y, z);
}
