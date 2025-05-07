#pragma once

// Headers
#include "core/core.hpp"
#include "core/framework.hpp"
#include "matrix.hpp"
#include "quaternion.hpp"

// Forward declarations
class vec3;

namespace Raytracing
{
    struct Transform
    {
    public:
	    Transform();
	    Transform(const vec3& translation, const Quaternion& rotation, const vec3& scailing);
        Transform(const glm::mat4x4& model);

	    Transform inverse();

	    void set_translation(const vec3& translation);
	    void set_rotation(const Quaternion& rotation);
        void set_rotation(vec3 axis, const double& angle);
	    void set_scaling(const vec3& scailing);

	    void set_recompute(bool recompute);
	    bool needs_recompute() const;

	    void cache_model();
	    Raytracing::Matrix44 get_model();
        void set_model(const Raytracing::Matrix44& model);

        static vec3 get_translation(const Raytracing::Matrix44& model);
        static Quaternion get_rotation(const Raytracing::Matrix44& model);
        static vec3 get_scaling(const Raytracing::Matrix44& model);

	     static const Raytracing::Matrix44 transform_matrix(const vec3& translation = vec3(0), vec3 axis = x_axis, const double& angle = 0, const vec3& scailing = vec3(1));

    private:
	    bool recompute = false;
	    vec3 translation;
	    Quaternion rotation;
	    vec3 scaling;
	    Raytracing::Matrix44 model;

	    Raytracing::Matrix44 compute_model();
    };
}
