#include "util.h"

namespace mel {

    const std::string get_current_date_time() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d-%H.%M.%S", &tstruct);
        return buf;
    }

    void print_uint_vec(uint32_vec vector) {
        for (auto it = vector.begin(); it != vector.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }

    void print_int_vec(int32_vec vector) {
        for (auto it = vector.begin(); it != vector.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }

    void print_double_vec(double_vec vector) {
        for (auto it = vector.begin(); it != vector.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }

    double sin_trajectory(double amplitude, double frequency, double time) {
        return amplitude * sin(2 * mel::PI * frequency * time);
    }
}