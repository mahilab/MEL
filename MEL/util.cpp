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

    double sin_trajectory(double amplitude, double frequency, double time) {
        return amplitude * sin(2 * mel::PI * frequency * time);
    }

    double pd_control_effort(double kp, double kd, double x_ref, double x, double xd_ref, double xd) {
        return kp * (x_ref - x) + kd * (xd_ref - xd);
    }

}