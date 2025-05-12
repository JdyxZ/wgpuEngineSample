#pragma once

// Headers
#include "core/core.hpp"
#include "hittables/hittable.hpp"
#include "utils/scene_stats.hpp"
#include "math/aabb.hpp"

// Forward declarations
class hittable_list;

class bvh_node : public Hittable 
{
public:
    // Hierarchy specs
    int depth;
    int nodes;

    bvh_node() = default; // Default constructor    

    // Creates an implicit copy of the hittable list, which we will modify. 
    // The lifetime of the copied list only extends until this constructor exits.
    bvh_node(hittable_list list, const optional<Raytracing::Matrix44>& model = nullopt);  

    bvh_node(vector<shared_ptr<Hittable>>& objects, size_t start, size_t end, bvh_stats& stats);
    bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;

    const bvh_stats get_stats() const;

private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    bvh_stats stats;

    static bool box_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b, int axis_index);
    static bool box_x_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b);
    static bool box_y_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b);
    static bool box_z_compare(const shared_ptr<Hittable>& a, const shared_ptr<Hittable>& b);
};




