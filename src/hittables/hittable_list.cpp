// Headers
#include "core/core.hpp"
#include "hittable_list.hpp"
#include "hittable.hpp"
#include "utils/interval.hpp"

hittable_list::hittable_list() {}

void hittable_list::add(shared_ptr<hittable_list> list)
{
    objects.insert(objects.end(), list->objects.begin(), list->objects.end());

}

void hittable_list::add(shared_ptr<Hittable> object)
{
    objects.push_back(object);
}

void hittable_list::clear()
{
    objects.clear();
}

size_t hittable_list::size() const
{
    return objects.size();
}

bool hittable_list::intersect(const shared_ptr<Ray>& r, Interval ray_t, shared_ptr<hit_record>& rec) const
{
    shared_ptr<hit_record> temp_rec = make_shared<hit_record>();
    bool hit_anything = false;
    auto closest_object_so_far = ray_t.max;

    for (const auto& object : objects)
    {
        if (object->hit(r, Interval(ray_t.min, closest_object_so_far), temp_rec))
        {
            hit_anything = true;
            closest_object_so_far = temp_rec->t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

shared_ptr<Hittable> hittable_list::operator[](int i) const
{
    return objects[i];
}
