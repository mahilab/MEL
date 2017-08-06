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

    double sin_wave(double amplitude, double frequency, double t) {
        return amplitude * sin(2 * PI * frequency * t);
    }

    double cos_wave(double amplitude, double frequency, double t) {
        return amplitude * cos(2 * PI * frequency * t);
    }

    double square_wave(double amplitude, double frequency, double t) {
        double sin = sin_wave(1, frequency, t);
        if (sin == 0)
            return 0;
        else if (sin > 0)
            return amplitude;
        else if (sin < 0)
            return -amplitude;
    }

    double triangle_wave(double amplitude, double frequency, double t) {
        return 2 * amplitude / PI * asin(sin(2 * PI * frequency * t));
    }

    double sawtooth_wave(double amplitude, double frequency, double t) {
        return -2 * amplitude / PI * atan(cos(PI * frequency * t) / sin(PI * frequency * t));
    }

    double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd) {
        return kp * (x_ref - x) + kd * (xd_ref - xd);
    }

    double saturate(double value, double max, double min) {
        if (value > max)
            return max;
        else if (value < min)
            return min;
        else
            return value;
    }

    double saturate(double value, double abs_max) {
        return saturate(value, abs_max, -abs_max);
    }

}