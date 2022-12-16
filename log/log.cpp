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

//using namespace std::literals::chrono_literals;
//static const constexpr auto kInterval = 8ms;
//static const constexpr auto kSpin = 500us;
static const int period_gap = 8 * 1000 * 1000;  // ms
//static const int period_spin = 100 * 1000;      // us

static inline int timespec_compare(const struct timespec *lhs, const struct timespec *rhs) {
    if (lhs->tv_sec < rhs->tv_sec) return -1;
    if (lhs->tv_sec > rhs->tv_sec) return 1;
    return lhs->tv_nsec - rhs->tv_nsec;
}

void Log::start() {
    std::string data_buff;
    while (Demo::isrun) {
//        auto now = std::chrono::steady_clock::now();

        clock_gettime(CLOCK_MONOTONIC,&time1);
        //        auto tick = now;

        clock_gettime(CLOCK_MONOTONIC,&period);

        period.tv_nsec += period_gap;
        while (period.tv_nsec >= 1000000000) {
            period.tv_nsec -= 1000000000;
            period.tv_sec++;
        }
        clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&period,NULL);
        {
            while (dobot_buffer.try_dequeue(data_buff)) {
                save_file << data_buff;
                moodycamel::ConcurrentQueue<std::string> temp;
                dobot_buffer.swap( temp);
            }
            while (serial_buffer.try_dequeue(data_buff)) {
                save_file << data_buff;
                moodycamel::ConcurrentQueue<std::string> temp;
                serial_buffer.swap( temp);
            }
            clock_gettime(CLOCK_MONOTONIC, &time2);
//            std::cout << "log time : "
//                      << (time2.tv_nsec-time1.tv_nsec) /1000.0/1000.0
//                      << "ms\n";

            //            now = std::chrono::steady_clock::now();
        }
    }
}
