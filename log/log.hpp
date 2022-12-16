#include <qcoreapplication.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <time.h>

#include "3rd-party/concurrentqueue.h"

class Log {
   public:
    ~Log() { save_file.close(); }
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    static Log& get_instance() {
        static Log instance;
        return instance;
    }

    void start();

   public:
    moodycamel::ConcurrentQueue<std::string> log_buffer;

   private:
    Log() {
        auto app_path = QCoreApplication::applicationDirPath();
        save_file.open(app_path.toStdString() + "/data.txt", std::ios::trunc);
    };
    std::ofstream save_file;
    struct timespec time1 = {0, 0};
    struct timespec time2 = {0, 0};
    struct timespec period = {0, 0};
};