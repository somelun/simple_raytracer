#pragma once

#include "vec3.h"
#include "ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class aabb {
public:
    aabb() {}
    aabb(const vec3& a, const vec3& b ) { min_ = a; max_ = b; }

    vec3 min() const { return min_; }
    vec3 max() const { return max_; }

    bool hit(const ray& r, float tmin, float tmax) const {
        for(int i = 0; i < 3; ++i) {
            float t0 = ffmin((min_[i] - r.origin()[i]) / r.direction()[i], (max_[i] - r.origin()[i]) / r.direction()[i]);
            float t1 = ffmax((min_[i] - r.origin()[i]) / r.direction()[i], (max_[i] - r.origin()[i]) / r.direction()[i]);

            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            if (tmax <= tmin) {
                return false;
            }
        }
        return true;
    }

private:
    vec3 min_;
    vec3 max_;
};

aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small( ffmin(box0.min().x(), box1.min().x()),
                ffmin(box0.min().y(), box1.min().y()),
                ffmin(box0.min().z(), box1.min().z()));
    vec3 big  ( ffmax(box0.max().x(), box1.max().x()),
                ffmax(box0.max().y(), box1.max().y()),
                ffmax(box0.max().z(), box1.max().z()));
    return aabb(small,big);
}
