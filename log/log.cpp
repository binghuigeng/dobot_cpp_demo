#include "log.hpp"

#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

#include "Demo.h"

using namespace std::literals::chrono_literals;
static const constexpr auto kInterval = 8ms;
static const constexpr auto kSpin = 500us;
static const int period_gap = 8 * 1000 * 1000;  // ms
static const int period_spin = 100 * 1000;      // us

static inline int timespec_compare(const struct timespec *lhs, const struct timespec *rhs) {
    if (lhs->tv_sec < rhs->tv_sec) return -1;
    if (lhs->tv_sec > rhs->tv_sec) return 1;
    return lhs->tv_nsec - rhs->tv_nsec;
}

void Log::start() {
    std::string data_buff;
    while (Demo::isrun) {
        auto now = std::chrono::steady_clock::now();

        auto tick = now;

        tick += kInterval;
        std::this_thread::sleep_until(tick);
        // std::this_thread::sleep_until(tick - kSpin);
        while (now < tick) {
            std::this_thread::yield();
            while (log_buffer.try_dequeue(data_buff)) {
                save_file << data_buff;
            }

            clock_gettime(CLOCK_MONOTONIC, &time2);
            std::cout << "log time : "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count() << "ms\n";
            now = std::chrono::steady_clock::now();
        }
    }
}
