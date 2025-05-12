// Headers
#include "core/core.hpp"
#include "bvh.hpp"
#include "hittables/hittable_list.hpp"
#include "utils/chrono.hpp"
#include "ray.hpp"

// Usings
using Raytracing::AABB;

bvh_node::bvh_node(hittable_list list, const optional<Raytracing::Matrix44>& model)
{
    stats = bvh_stats();

    stats.bvh_chrono.start();

    auto instance = bvh_node(list.objects, 0, list.objects.size(), stats);

    stats.bvh_chrono.end();

    stats.bvh_depth = instance.depth + 1;
    stats.bvh_nodes += instance.nodes + 1;

    instance.stats = stats;

    *this = instance;

    set_model(model);
}

bvh_node::bvh_node(vector<shared_ptr<Hittable>>& objects, size_t start, size_t end, bvh_stats& stats)
{
    // Define hittable type
    type = BVH_NODE;

    // Build the bounding box of the span of source objects.
    bbox = original_bbox = AABB::empty();
    for (size_t object_index = start; object_index < end; object_index++)
    {
        bbox = original_bbox = AABB(original_bbox.value(), objects[object_index]->get_bbox());
    }

    int axis = original_bbox.value().longest_axis();

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1)
    {
        // Assign object to leaves
        auto object = objects[start];
        left = object;
        right = nullptr;

        // Calculate depth and nodes
        auto node_depth = bvh_stats::get_bvh_depth(object);
        depth = node_depth == 0 ? 0 : node_depth + 1;
        nodes = 1; // Right node is null, so only one node is counted

        // Process object for bvh stats
        stats.add(object);
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
        stats.add(left_object);
        stats.add(right_object);
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

bool bvh_node::hit(const Ray& r, const Interval& ray_t, hit_record& rec) const
{
    const Ray local_ray = transformed ? transform_ray(r) : r;

    bool special_case = (right == nullptr) && left->is_bvh_tree(); // We assume left node cannot be nullptr !!!

    if (!special_case && !bbox.value().hit(r, ray_t))
        return false;

    bool hit_left = false, hit_right = false;

    hit_left = left->hit(r, ray_t, rec);

    if (right != nullptr)
        hit_right = right->hit(r, Interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

    if (transformed && (hit_left || hit_right))
        transform_hit_record(rec);

    return hit_left || hit_right;
}

const bvh_stats bvh_node::get_stats() const
{
    return stats;
}

bool bvh_node::box_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b, int axis_index)
{
    auto a_axis_interval = a->get_bbox().axis_interval(axis_index);
    auto b_axis_interval = b->get_bbox().axis_interval(axis_index);
    return a_axis_interval.min < b_axis_interval.min;
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
