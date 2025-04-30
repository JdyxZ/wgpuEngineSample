// Headers
#include "core/core.hpp"
#include "box.hpp"
#include "quad.hpp"
#include "hittable_list.hpp"
#include "math/interval.hpp"
#include "math/matrix.hpp"
#include "utils/utilities.hpp"
#include "ray.hpp"

// Usings
using Raytracing::Material;
using Raytracing::Matrix44;
using Raytracing::AABB;

Box::Box(point3 p0, point3 p1, const shared_ptr<Material>& material, const optional<Matrix44>& model) : material (material)
{
	// Validate that p0 and p1 are not aligned in any coordinate (this would define a line or a point instead of a box)
	if (p0.x == p1.x || p0.y == p1.y || p0.z == p1.z)
	{
		string error = Logger::error("BOX", "Points p0 and p1 are aligned in at least one coordinate. A box cannot be formed.");
		throw std::invalid_argument(error);
	}

	// Define Box 
	type = BOX;
	this->p0 = p0;
	this->p1 = p1;

    // Set model
    set_model(model);

	// Construct the two opposite vertices with the minimum and maximum coordinates.
	auto min = min_vector(p0, p1);
	auto max = max_vector(p0, p1);

	// Construct the coordinate system of the box.
	auto dx = vec3(max.x - min.x, 0, 0);
	auto dy = vec3(0, max.y - min.y, 0);
	auto dz = vec3(0, 0, max.z - min.z);

	// Construct the box from the six quads that make up its sides.
	auto quad1 = make_shared<Quad>(point3(min.x, min.y, max.z), dx, dy, material); // front
	auto quad2 = make_shared<Quad>(point3(max.x, min.y, max.z), -dz, dy, material); // right
	auto quad3 = make_shared<Quad>(point3(max.x, min.y, min.z), -dx, dy, material); // back
	auto quad4 = make_shared<Quad>(point3(min.x, min.y, min.z), dz, dy, material); // left
	auto quad5 = make_shared<Quad>(point3(min.x, max.y, max.z), dx, -dz, material); // top
	auto quad6 = make_shared<Quad>(point3(min.x, min.y, min.z), dx, dz, material); // bottom

	// BVH
	auto sides_list = hittable_list();
	sides_list.add(quad1);
	sides_list.add(quad2);
	sides_list.add(quad3);
	sides_list.add(quad4);
	sides_list.add(quad5);
	sides_list.add(quad6);
	sides = bvh_node(sides_list);

    // Set bvh bounding box as the bounding box of the box
    bbox = sides.bounding_box();
}

bool Box::hit(const Ray& r, Interval ray_t, hit_record& rec) const
{
    if (!transformed)
        return sides.hit(r, ray_t, rec);

    const Ray local_ray = transform_ray(r);

    const bool hit = sides.hit(local_ray, ray_t, rec);

    if (hit)
        transform_hit_record(rec);

    return hit;
}

AABB Box::bounding_box() const
{
	return bbox;
}

const bvh_stats Box::get_stats() const
{
    return sides.get_stats();
}
