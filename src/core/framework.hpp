#pragma once

// Headers
#include "math/vec3.hpp"

// Axis definitions (right-handed coordinate system)
const Raytracing::axis x_axis = vec3(1, 0, 0);
const Raytracing::axis y_axis = vec3(0, 1, 0);
const Raytracing::axis z_axis = vec3(0, 0, -1);

const Raytracing::axis flipped_x_axis = vec3(-1, 0, 0);
const Raytracing::axis flipped_y_axis = vec3(0, -1, 0);
const Raytracing::axis flipped_z_axis = vec3(0, 0, 1);
