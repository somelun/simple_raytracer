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
#include "aarect.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define RENDER_LOG
#define TIME_PROFILER

#ifdef TIME_PROFILER
#include "timer.h"
#endif


vec3 color(const ray& r, hittable* world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
             return emitted + attenuation * color(scattered, world, depth + 1);
        else {
            return emitted;
        }
    } else {
        return vec3(0,0,0);
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

hittable *simple_light() {
    texture *pertext = new noise_texture(4);
    hittable **list = new hittable*[4];
    list[0] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    list[2] = new sphere(vec3(0, 7, 0), 2,
        new diffuse_light(new constant_texture(vec3(4,4,4))));
    list[3] = new xy_rect(3, 5, 1, 3, -2,
        new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hittable_list(list,4);
}

hittable *cornell_box() {
    hittable **list = new hittable*[6];
    int i = 0;
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

    return new hittable_list(list,i);
}

int main() {
    std::ofstream fout;
    fout.open("./image.ppm");

    if (!fout.is_open()) {
        std::cout << "Can't create file!" << std::endl;
        return EXIT_FAILURE;
    }

    int nx = 800;
    int ny = 600;
    int ns = 100;

    fout <<  "P3\n" << nx << " " << ny << "\n255\n";

    // hittable *world = random_scene();
    // hittable *world = two_spheres();
    // hittable *world = two_perlin_spheres();
    // hittable *world = earth();
    hittable *world = cornell_box();

    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;

    camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);

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
