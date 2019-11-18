#pragma once

#include "perlin.h"

class texture {
public:
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() {}
    constant_texture(vec3 c) : color_(c) {}

    virtual vec3 value(float u, float v, const vec3& p) const override {
        return color_;
    }

private:
    vec3 color_;
};

class checker_texture : public texture {
public:
    checker_texture() {}
    checker_texture(texture* t0, texture* t1) : odd_(t0), even_(t1) {}

    virtual vec3 value(float u, float v, const vec3& p) const override {
        float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0) {
            return odd_->value(u, v, p);
        } else {
            return even_->value(u, v, p);
        }
    }

private:
    texture* odd_;
    texture* even_;
};

class noise_texture : public texture {
public:
    noise_texture() {}
    noise_texture(float sc) : scale_(sc) {}

    virtual vec3 value(float u, float v, const vec3& p) const {
        // return vec3(1, 1, 1) * noise_.turb(scale_ * p);
        return vec3(1, 1, 1) * 0.5f * (1 + sin(scale_ * p.z() + 10 * noise_.turb(p)));
    }

private:
    perlin noise_;
    float scale_{5.0f};
};
