#include "util.h"

const std::string get_current_date_time() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d-%H.%M.%S", &tstruct);
    return buf;
}

void print_uint_vec(uint_vec vector) {
    for (auto it = vector.begin(); it != vector.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void print_int_vec(int_vec vector) {
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