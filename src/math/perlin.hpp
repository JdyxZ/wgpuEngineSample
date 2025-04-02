#pragma once

// Headers
#include "vec3.hpp"

class Perlin 
{
public:
    Perlin();
    
    double noise(const point3& p) const;
    double turbulance(const point3& p, int depth) const;
   
private:
    static const int point_count = 256;
    vec3 randvec[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    static void perlin_generate_perm(int* p);
    static void permute(int* p, int n);
    static double perlin_interpolation(const vec3 c[2][2][2], double u, double v, double w);
};


