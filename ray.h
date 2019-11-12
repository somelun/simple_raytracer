#pragma once

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const vec3& a, const vec3& b, float t = 0.0f) {
        A = a;
        B = b;
        time_ = t;
    };

    vec3 origin() const { return A; }
    vec3 direction() const { return B; }
    float time() const { return time_; }

    vec3 point_at_parameter(float t) const { return A + B * t; }

private:
    vec3 A;
    vec3 B;
    float time_;
};
