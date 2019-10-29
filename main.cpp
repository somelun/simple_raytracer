#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"


vec3 color(const ray& r, hitable *world, int depth) {
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

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            float choose_mat = drand48();
            vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) { //diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48())));
                } else if (choose_mat < 0.95) { //metal
                    list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())),  0.5 * drand48()));
                } else {    //glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(1, 1, 0), 1.0, new metal(vec3(0.5, 0.5, 0.5), 0.0));

    return new hitable_list(list, i);
}

void print_progress_bar(int perc) {
    std::cout << "[" << perc << "%]\r";
    std::cout.flush();
}

int main() {
    std::ofstream fout;
    fout.open("/Users/lun/Desktop/image.ppm");

    if (!fout.is_open()) {
        std::cout << "Can't create file!" << std::endl;
        return 1;
    }

    int nx = 800;
    int ny = 600;
    int ns = 40;

    fout <<  "P3\n" << nx << " " << ny << "\n255\n";

    hitable *world = random_scene();

    vec3 lookfrom(13, 4, 6);
    vec3 lookat(0, 0, -1);
    float dist_to_focus = (lookat - lookfrom).length();
    float aperture = 0.0;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx)/float(ny), aperture, dist_to_focus);

    unsigned int iters_for_new_perc = nx * ny * ns / 100;
    unsigned int current_iter = 0;
    int perc = 0;
    print_progress_bar(perc);

    for (int i = ny - 1; i >= 0; --i) {
        for (int j = 0; j < nx; ++j) {

            vec3 col(0.0, 0.0, 0.0);

            for (int k = 0; k < ns; ++k) {

                current_iter++;
                if (current_iter >= iters_for_new_perc) {
                    current_iter = 0;
                    perc++;
                    print_progress_bar(perc);
                }

                float u = float(j + drand48()) / float(nx);
                float v = float(i + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                // vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            fout <<  ir << " " << ig << " " << ib << "\n";
        }
    }

    fout.close();

    return 0;
}
