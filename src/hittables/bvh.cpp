// Headers
#include "core/core.hpp"
#include "bvh.hpp"
#include "hittables/hittable_list.hpp"
#include "utils/chrono.hpp"
#include "math/aabb.hpp"

// Usings
using Raytracing::AABB;

bvh_node::bvh_node(hittable_list list)
{
    stats = make_shared<bvh_stats>();

    stats->bvh_chrono->start();

    auto instance = bvh_node(list.objects, 0, list.objects.size(), stats);

    stats->bvh_chrono->end();

    stats->bvh_depth = instance.depth + 1;
    stats->bvh_nodes += instance.nodes + 1;

    instance.stats = stats;

    *this = instance;
}

bvh_node::bvh_node(vector<shared_ptr<Hittable>>& objects, size_t start, size_t end, shared_ptr<bvh_stats>& stats)
{
    // Define hittable type
    type = BVH_NODE;

    // Build the bounding box of the span of source objects.
    bbox = make_shared<AABB>(AABB::empty());
    for (size_t object_index = start; object_index < end; object_index++)
        bbox = make_shared<AABB>(*bbox, *objects[object_index]->bounding_box());

    int axis = bbox->longest_axis();

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1)
    {
        // Assign object to leaves
        auto object = objects[start];
        left = right = object;

        // Calculate depth and nodes
        auto node_depth = bvh_stats::get_bvh_depth(object);
        depth = node_depth == 0 ? 0 : node_depth + 1;
        nodes = 1; // Same object for left and and right leaf

        // Process object for bvh stats
        stats->add(object);
    }
    else if (object_span == 2)
    {
        //Assign objects to leaves
        auto left_object = objects[start];
        auto right_object = objects[start + 1];
        left = left_object;
        right = right_object;

        // Calculate depth and nodes
        auto node_depth = std::max(bvh_stats::get_bvh_depth(left_object), bvh_stats::get_bvh_depth(right_object));
        depth = node_depth == 0 ? 0 : node_depth + 1;
        nodes = 2;

        // Process objects for bvh stats
        stats->add(left_object);
        stats->add(right_object);
    }
    else
    {
        // Sort the objects based on the chosen axis
        std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

        // Create nodes
        auto mid = start + object_span / 2;
        auto left_node = make_shared<bvh_node>(objects, start, mid, stats);
        auto right_node = make_shared<bvh_node>(objects, mid, end, stats);

        // Update depth and nodes based on the children
        depth = std::max(left_node->depth, right_node->depth) + 1;
        nodes = 2 + left_node->nodes + right_node->nodes;

        // Assign nodes to hittable pointers
        left = left_node;
        right = right_node;
    }
}

bool bvh_node::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    const auto local_ray = transformed ? transform_ray(r) : r;

    if (!bbox->hit(local_ray, ray_t))
        return false;

    const bool hit_left = left->hit(local_ray, ray_t, rec);
    const bool hit_right = right->hit(local_ray, Interval(ray_t.min, hit_left ? rec->t : ray_t.max), rec);

    if (transformed && (hit_left || hit_right))
        transform_hit_record(rec);

    return hit_left || hit_right;
}

shared_ptr<AABB> bvh_node::bounding_box() const
{
    return bbox;
}

shared_ptr<bvh_stats> bvh_node::get_stats() const
{
    return stats;
}

bool bvh_node::box_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b, int axis_index)
{
    auto a_axis_interval = a->bounding_box()->axis_interval(axis_index);
    auto b_axis_interval = b->bounding_box()->axis_interval(axis_index);
    return a_axis_interval->min < b_axis_interval->min;
}

bool bvh_node::box_x_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b)
{
    return box_compare(a, b, 0);
}

bool bvh_node::box_y_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b)
{
    return box_compare(a, b, 1);
}

bool bvh_node::box_z_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b)
{
    return box_compare(a, b, 2);
}
