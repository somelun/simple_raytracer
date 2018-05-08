#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"


vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while(p.squared_length() >= 1.0);

    return p;
}

vec3 color(const ray& r, hitable *world) {
    hit_record rec;
    if (world->hit(r, 0.0, MAXFLOAT, rec)) {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5  * color(ray(rec.p, target - rec.p), world);
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

void print_progress_bar(int perc) {
    std::cout.flush();
    std::cout << "[" << perc << "%]\r";
    std::cout.flush();
}

int main() {
    std::ofstream fout;
    fout.open("/Users/lun/Work/cpp/raytraceinweek/test.ppm");

    if (!fout.is_open()) {
        std::cout << "Can't create file!" << std::endl;
        return 1;
    }

    int nx = 1024;
    int ny = 512;
    int ns = 100;
    
    fout <<  "P3\n" << nx << " " << ny << "\n255\n";

    hitable *list[2];
    list[0] = new sphere(vec3(0, 0, -1), 0.5);
    list[1] = new sphere(vec3(0, -100.5, -1), 100);

    camera cam;
    hitable *world = new hitable_list(list, 2);

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
                col += color(r, world);
            }
            col /= float(ns);

            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            fout <<  ir << " " << ig << " " << ib << "\n";
        }
    }

    fout.close();

    return 0;
}
