#pragma once

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
        float sines = sin(10 * p.x()) * sin(10 * p.y()) *sin(10 * p.z());
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
