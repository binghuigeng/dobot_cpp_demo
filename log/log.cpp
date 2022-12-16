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

// using namespace std::literals::chrono_literals;
// static const constexpr auto kInterval = 8ms;
// static const constexpr auto kSpin = 500us;
static const int period_gap = 4 * 1000 * 1000;  // ms
// static const int period_spin = 100 * 1000;      // us
static const std::string place_holder{"100000,100000,100000,100000,100000,100000,"};

static inline int timespec_compare(const struct timespec *lhs, const struct timespec *rhs) {
    if (lhs->tv_sec < rhs->tv_sec) return -1;
    if (lhs->tv_sec > rhs->tv_sec) return 1;
    return lhs->tv_nsec - rhs->tv_nsec;
}

void Log::start() {
    std::string dobot_data_buff;
    std::string serial_data_buff;
    static long long number_count{0};
    while (Demo::isrun) {
        //        auto now = std::chrono::steady_clock::now();

        clock_gettime(CLOCK_MONOTONIC, &time1);
        //        auto tick = now;

        clock_gettime(CLOCK_MONOTONIC, &period);

        period.tv_nsec += period_gap;
        while (period.tv_nsec >= 1000000000) {
            period.tv_nsec -= 1000000000;
            period.tv_sec++;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period, NULL);
        {
            save_file << number_count++ << ", ";
            while (dobot_buffer.try_dequeue(dobot_data_buff)) {

//                 moodycamel::ConcurrentQueue<std::string> temp;
//                 dobot_buffer.swap( temp);
                 break;
            }

            while (serial_buffer.try_dequeue(serial_data_buff)) {
//                 moodycamel::ConcurrentQueue<std::string> temp;
//                 serial_buffer.swap( temp);
                 break;
            }

            if (dobot_data_buff.length() == 0){
                dobot_data_buff = place_holder;
            }
            if (serial_data_buff.length() == 0){
                serial_data_buff = place_holder;
            }
            save_file << dobot_data_buff
                      << serial_data_buff
                      << "\n";
            clock_gettime(CLOCK_MONOTONIC, &time2);
            std::cout << number_count << " log time : " << (time2.tv_nsec - time1.tv_nsec) / 1000.0 / 1000.0 << "ms\n";

            //            now = std::chrono::steady_clock::now();
        }
    }
}
