#include "util.h"


namespace mel {

    const std::string get_ymdhms() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d-%H.%M.%S", &tstruct);
        return buf;
    }

    std::string namify(std::string name) {
        return "<" + name + ">";
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

    /*double_vec mat_vec_multiply(double_mat A, double_vec b) {
        double_vec c(A.size(),0);
        if (A[0].size() == b.size()) {
            for (int i = 0; i < A.size(); ++i) {
                for (int j = 0; j < A[i].size(); ++j) {
                    c[i] += A[i][j] * b[j];
                }
            }
        }
        else {
            print("FATAL ERROR: inner dimensions must match");
        }
        return c;
    }*/


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
        abs_max = abs(abs_max);
        return saturate(value, abs_max, -abs_max);
    }

}