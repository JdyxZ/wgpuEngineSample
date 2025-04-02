// Headers
#include "core/core.hpp"
#include "hittables/hittable_list.hpp"
#include "utils/chrono.hpp"
#include "bvh.hpp"
#include "aabb.hpp"

// Usings
using Raytracing::AABB;

bvh_node::bvh_node(hittable_list list)
{
    chrono = make_shared<Chrono>();

    chrono->start();

    auto instance = bvh_node(list.objects, 0, list.objects.size());

    chrono->end();

    instance.type = BVH_NODE;
    instance.chrono = chrono;

    *this = instance;
}

bvh_node::bvh_node(vector<shared_ptr<Hittable>>& objects, size_t start, size_t end)
{
    // Build the bounding box of the span of source objects.
    bbox = make_shared<AABB>(AABB::empty);
    for (size_t object_index = start; object_index < end; object_index++)
        bbox = make_shared<AABB>(bbox, objects[object_index]->bounding_box());

    int axis = bbox->longest_axis();

    auto comparator = (axis == 0) ? box_x_compare
        : (axis == 1) ? box_y_compare
        : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1)
    {
        left = right = objects[start];
        depth = 0;
        nodes = 1; // Same object for left and and right leaf
    }
    else if (object_span == 2)
    {
        left = objects[start];
        right = objects[start + 1];
        depth = 0;
        nodes = 1;
    }
    else
    {
        // Sort the objects based on the chosen axis
        std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

        // Create nodes
        auto mid = start + object_span / 2;
        auto left_node = make_shared<bvh_node>(objects, start, mid);
        auto right_node = make_shared<bvh_node>(objects, mid, end);

        // Update depth and nodes based on the children
        depth = std::max(left_node->depth, right_node->depth) + 1;
        nodes = 1 + left_node->nodes + right_node->nodes;

        // Assign nodes to hittable pointers
        left = left_node;
        right = right_node;
    }
}

bool bvh_node::hit(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    if (!bbox->hit(r, ray_t))
        return false;

    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right = right->hit(r, Interval(ray_t.min, hit_left ? rec->t : ray_t.max), rec);

    return hit_left || hit_right;
}

shared_ptr<AABB> bvh_node::bounding_box() const
{
    return bbox;
}

const shared_ptr<Chrono> bvh_node::bvh_chrono() const
{
    return chrono;
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
