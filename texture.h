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

    virtual vec3 value(float u, float v, const vec3& p) const override {
        // return vec3(1, 1, 1) * noise_.turb(scale_ * p);
        return vec3(1, 1, 1) * 0.5f * (1 + sin(scale_ * p.z() + 10 * noise_.turb(p)));
    }

private:
    perlin noise_;
    float scale_{5.0f};
};

class image_texture : public texture {
public:
    image_texture() {}
    image_texture(unsigned char* pixels, int A, int B) : data_(pixels), nx_(A), ny_(B) {}

    virtual vec3 value(float u, float v, const vec3& p) const override {
        int i = (u) * nx_;
        int j = (1 - v) * ny_ - 0.001f;

        if (i < 0) {
            i = 0;
        }
        if (j < 0) {
            j = 0;
        }
        if (i > nx_ - 1) {
            i = nx_ - 1;
        }
        if (j > ny_ - 1) {
            j = ny_ - 1;
        }

        float r = int(data_[3 * i + 3 * nx_ * j]) / 255.0f;
        float g = int(data_[3 * i + 3 * nx_ * j +1]) / 255.0f;
        float b = int(data_[3 * i + 3 * nx_ * j +2]) / 255.0f;

        // std::cout << "rgb: " << r << ", " << g << ", " << b << "\n";

        return vec3(r, g, b);
    }

private:
    unsigned char* data_;
    int nx_, ny_;
};
