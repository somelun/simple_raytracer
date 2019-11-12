#pragma once

#include <chrono>
#include <iostream>

class timer {
    using clock_t = std::chrono::high_resolution_clock;
    using microseconds = std::chrono::microseconds;
public:
    timer()
        : start_(clock_t::now()) {}

    ~timer() {
        const auto finish = clock_t::now();
        const auto us = std::chrono::duration_cast<microseconds>(finish - start_).count();
        std::cout << std::endl << us / 100000 << " seconds" << std::endl;
    }

private:
    const clock_t::time_point start_;
};