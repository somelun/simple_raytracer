#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hittable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "bhv.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define RENDER_LOG
#define TIME_PROFILER

#ifdef TIME_PROFILER
#include "timer.h"
#endif


vec3 color(const ray& r, hittable* world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        } else {
            return vec3(0,0,0);
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

hittable* random_scene() {
    int n = 50000;
    hittable **list = new hittable*[n + 1];
    texture* checker = new checker_texture(new constant_texture(vec3(0.2f, 0.3f, 0.1f)), new constant_texture(vec3(0.9f, 0.9f, 0.9f)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -10; a < 10; ++a) {
        for (int b = -10; b < 10; ++b) {
            float choose_mat = drand48();
            vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) { //diffuse
                    list[i++] = new moving_sphere(center, center + vec3(0, 0.5f * drand48(), 0), 0.0f, 1.0f, 0.2f, new lambertian(new constant_texture(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48()))));
                } else if (choose_mat < 0.95f) { //metal
                    list[i++] = new sphere(center, 0.2f, new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())),  0.5 * drand48()));
                } else {    //glass
                    list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5f));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4f, 0.2f, 0.1f))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));

    return new hittable_list(list, i);
}

hittable* two_spheres() {
    texture* checker = new checker_texture(new constant_texture(vec3(0.2f, 0.3f, 0.1f)), new constant_texture(vec3(0.9f, 0.9f, 0.9f)));
    int n = 50;
    hittable** list = new hittable*[n + 1];
    list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
    list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));

    return new hittable_list(list, 2);
}

hittable* two_perlin_spheres() {
    texture* pertext = new noise_texture();
    hittable** list = new hittable*[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    return new hittable_list(list, 2);
}

hittable* earth() {
    int nx, ny, nn;
    unsigned char* tex_data = stbi_load("/Users/lun/Work/cpp/simple_raytracer/earthmap.jpg", &nx, &ny, &nn, 0);
    material* mat = new lambertian(new image_texture(tex_data, nx, ny));

    return new sphere(vec3(0,0, 0), 2, mat);
}

int main() {
    std::ofstream fout;
    fout.open("./image.ppm");

    if (!fout.is_open()) {
        std::cout << "Can't create file!" << std::endl;
        return EXIT_FAILURE;
    }

    int nx = 640;
    int ny = 480;
    int ns = 10;

    fout <<  "P3\n" << nx << " " << ny << "\n255\n";

    // hittable *world = random_scene();
    // hittable *world = two_spheres();
    // hittable *world = two_perlin_spheres();
    hittable *world = earth();

    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0f;//(lookat - lookfrom).length();
    float aperture = 0.0;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx)/float(ny), aperture, dist_to_focus, 0.0f, 1.0f);

#ifdef RENDER_LOG
    unsigned iters_for_new_perc = nx * ny * ns / 10000;
    unsigned int current_iter = 0;
#endif

#ifdef TIME_PROFILER
    timer t;
#endif

    for (int i = ny - 1; i >= 0; --i) {
        for (int j = 0; j < nx; ++j) {

            vec3 col(0.0f, 0.0f, 0.0f);

            for (int k = 0; k < ns; ++k) {

#ifdef RENDER_LOG
                fprintf(stderr,"\rRendering: %5.2f%%", (float)(current_iter / iters_for_new_perc) / 100);
                current_iter++;
#endif

                float u = float(j + drand48()) / float(nx);
                float v = float(i + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99f * col[0]);
            int ig = int(255.99f * col[1]);
            int ib = int(255.99f * col[2]);

            fout <<  ir << " " << ig << " " << ib << "\n";
        }
    }

    fout.close();

    return EXIT_SUCCESS;
}
