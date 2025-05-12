// Headers
#include "core/core.hpp"
#include "hittable_list.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "math/interval.hpp"

hittable_list::hittable_list()
{
    type = HITTABLE_LIST;
    bbox = original_bbox = Raytracing::AABB::empty();
}

hittable_list::hittable_list(const vector<shared_ptr<Hittable>>& list)
{
    type = HITTABLE_LIST;
    objects = list;
    bbox = original_bbox = Raytracing::AABB::empty();

    for(auto& object : objects)
        original_bbox = Raytracing::AABB(original_bbox.value(), object->get_bbox());

    recompute_bbox();
}

void hittable_list::add(const hittable_list& list)
{
    objects.insert(objects.end(), list.objects.begin(), list.objects.end());

    for (auto& object : list.objects)
        original_bbox = Raytracing::AABB(original_bbox.value(), object->get_bbox());

    recompute_bbox();
}

void hittable_list::add(const shared_ptr<Hittable>& object)
{
    objects.push_back(object);
    original_bbox = Raytracing::AABB(original_bbox.value(), object->get_bbox());
    recompute_bbox();
}

void hittable_list::add(const vector<shared_ptr<Hittable>>& list)
{
    objects.insert(objects.end(), list.begin(), list.end());

    for (auto& object : list)
        original_bbox = Raytracing::AABB(original_bbox.value(), object->get_bbox());

    recompute_bbox();
}

void hittable_list::clear()
{
    objects.clear();
    bbox = original_bbox = Raytracing::AABB::empty();
}

size_t hittable_list::size() const
{
    return objects.size();
}

void hittable_list::reserve(size_t size)
{
    objects.reserve(size);
}

bool hittable_list::hit(const Ray& r, const Interval& ray_t, hit_record& rec) const
{
    const Ray local_ray = transformed ? transform_ray(r) : r;

    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_object_so_far = ray_t.max;

    for (const auto& object : objects)
    {
        if (object->hit(r, Interval(ray_t.min, closest_object_so_far), temp_rec))
        {
            hit_anything = true;
            closest_object_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    if (transformed && hit_anything)
        transform_hit_record(rec);

    return hit_anything;
}

shared_ptr<Hittable> hittable_list::operator[](int i) const
{
    return objects[i];
}
