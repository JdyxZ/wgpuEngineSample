#pragma once

// Headers
#include "core/core.hpp"
#include "hittable.hpp"
#include "math/aabb.hpp"

// Forward declarations
class Hittable;
struct Ray;
struct Interval;
struct hit_record;

class hittable_list : public Hittable
{
public:
    vector<shared_ptr<Hittable>> objects; // Primitives 

    hittable_list();
    hittable_list(const vector<shared_ptr<Hittable>>& list);

    void add(const hittable_list& list);
    void add(const shared_ptr<Hittable>& object);
    void add(const vector<shared_ptr<Hittable>>& list);
    void clear();
	size_t size() const;
    void reserve(size_t size);
    bool hit(const Ray& r, const Interval& ray_t, hit_record& rec) const override;
  
	shared_ptr<Hittable> operator[](int i) const;
};

