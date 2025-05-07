// Headers
#include "core/core.hpp"
#include "transform.hpp"
#include "vec3.hpp"

// Framework headers
#include "framework/math/transform.h"

// Usings
using Raytracing::Matrix44;

Raytracing::Transform::Transform()
{
	translation = vec3(0.0);
	rotation = Quaternion(1.0, 0.0, 0.0, 0.0);
	scaling = vec3(1.0);
	recompute = true;
}

Raytracing::Transform::Transform(const vec3& translation, const Quaternion& rotation, const vec3& scailing)
{
	this->translation = vec3(translation);
	this->rotation = Quaternion(rotation);
	this->scaling = vec3(scailing);
	recompute = true;

	this->cache_model();
}

Raytracing::Transform::Transform(const glm::mat4x4& model)
{
    ::Transform framework_transform = ::Transform::mat4_to_transform(model);

    vec3 translation = framework_transform.get_position();
    Quaternion rotation = framework_transform.get_rotation();
    vec3 scailing = framework_transform.get_scale();

    *this = Transform(translation, rotation, scailing);
}

Raytracing::Transform Raytracing::Transform::inverse()
{
	auto inverse_transform = Raytracing::Transform();

	inverse_transform.set_scaling
	(
			vec3
			(
			std::fabs(scaling.x) < kEpsilon ? 0.0 : 1.0 / scaling.x,
			std::fabs(scaling.y) < kEpsilon ? 0.0 : 1.0 / scaling.y,
			std::fabs(scaling.z) < kEpsilon ? 0.0 : 1.0 / scaling.z
			)
	);

	inverse_transform.set_rotation(rotation.conjugate());

	inverse_transform.set_translation(inverse_transform.rotation * (inverse_transform.scaling * (translation * -1.0)));

	return inverse_transform;
}

void Raytracing::Transform::set_translation(const vec3& translation)
{
	this->translation = vec3(translation);
	recompute = true;
}

void Raytracing::Transform::set_rotation(const Quaternion& rotation)
{
	this->rotation = Quaternion(rotation);
	recompute = true;
}

void Raytracing::Transform::set_rotation(vec3 axis, const double& angle)
{
    auto radians = degrees_to_radians(angle);
    axis.normalize();

    this->rotation = Quaternion(axis, radians);
    recompute = true;
}

void Raytracing::Transform::set_scaling(const vec3& scailing)
{
	this->scaling = vec3(scailing);
	recompute = true;
}

void Raytracing::Transform::set_recompute(bool recompute)
{
	this->recompute = recompute;
}

bool Raytracing::Transform::needs_recompute() const
{
	return recompute;
}

void Raytracing::Transform::cache_model()
{
	model = get_model();
}

Matrix44 Raytracing::Transform::get_model()
{
	if (!recompute)
		return model;

	auto model = compute_model();
	recompute = false;

	return model;
}

void Raytracing::Transform::set_model(const Raytracing::Matrix44& model)
{
    // Extract translation
    vec3 translation = get_translation(model);

    // Extract scaling
    vec3 scaling = get_scaling(model);

    // Extract rotation
    Quaternion rotation = get_rotation(model);

    // Set internal values
    this->translation = translation;
    this->scaling = scaling;
    this->rotation = rotation;
    this->recompute = false; // The model is now in sync
    this->model = model;
}

vec3 Raytracing::Transform::get_translation(const Raytracing::Matrix44& model)
{
    return vec3(model[0][3], model[1][3], model[2][3]);
}

Quaternion Raytracing::Transform::get_rotation(const Raytracing::Matrix44& model)
{
    // Extract scaled axes
    vec3 x_axis(model[0][0], model[1][0], model[2][0]);
    vec3 y_axis(model[0][1], model[1][1], model[2][1]);
    vec3 z_axis(model[0][2], model[1][2], model[2][2]);

    // Get scaling
    vec3 scaling = get_scaling(model);

    // Normalize axes to get rotation matrix
    x_axis /= scaling.x;
    y_axis /= scaling.y;
    z_axis /= scaling.z;

    // Convert to Quaternion
    Matrix33 rot_mat(
        x_axis.x, y_axis.x, z_axis.x,
        x_axis.y, y_axis.y, z_axis.y,
        x_axis.z, y_axis.z, z_axis.z
    );

    return Quaternion(rot_mat);
}

vec3 Raytracing::Transform::get_scaling(const Raytracing::Matrix44& model)
{
    // Extract scaled axes
    vec3 x_axis(model[0][0], model[1][0], model[2][0]);
    vec3 y_axis(model[0][1], model[1][1], model[2][1]);
    vec3 z_axis(model[0][2], model[1][2], model[2][2]);

    // Extract scaling
    double sx = x_axis.length();
    double sy = y_axis.length();
    double sz = z_axis.length();

    return vec3(sx, sy, sz);
}

Matrix44 Raytracing::Transform::compute_model()
{
	// Get the center of the coordinate system 
	point3 local_center = translation;

	// Rotate basis vectors of the coordinate system
	vec3 local_x_axis = rotation * x_axis;
	vec3 local_y_axis = rotation * y_axis;
	vec3 local_z_axis = rotation * flipped_z_axis;

	// Scale basis vectors of the coordinate system
	local_x_axis *= scaling.x;
	local_y_axis *= scaling.y;
	local_z_axis *= scaling.z;

	// Create transformation matrix
	return Matrix44
	(
		local_x_axis.x, local_y_axis.x, local_z_axis.x, local_center.x,
		local_x_axis.y, local_y_axis.y, local_z_axis.y, local_center.y,
		local_x_axis.z, local_y_axis.z, local_z_axis.z, local_center.z,
		0.0, 0.0, 0.0, 1.0
	);
}

const Matrix44 Raytracing::Transform::transform_matrix(const vec3& translation, vec3 axis, const double& angle, const vec3& scailing)
{
	auto radians = degrees_to_radians(angle);
	axis.normalize();

	Quaternion rotation = Quaternion(axis, radians);
	Transform transform = Transform(translation, rotation, scailing);
	
	auto model = transform.get_model();

	return model;
}
