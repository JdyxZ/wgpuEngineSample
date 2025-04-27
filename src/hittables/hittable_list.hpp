#pragma once

// Headers
#include "core/core.hpp"

// Forward declarations
class Hittable;
class Ray;
class Interval;
class hit_record;

class hittable_list
{
public:
    vector<shared_ptr<Hittable>> objects; // Primitives 

    hittable_list();

    void add(shared_ptr<hittable_list> list);
    virtual void add(shared_ptr<Hittable> object);
    virtual void clear();
	size_t size() const;
    void reserve(size_t size);
    bool intersect(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const;
  
	shared_ptr<Hittable> operator[](int i) const;
};

