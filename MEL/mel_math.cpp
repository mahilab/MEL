#include "mel_math.h"

namespace mel {
    
    namespace math {

        double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd) {
            return kp * (x_ref - x) + kd * (xd_ref - xd);
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
            else
                return -amplitude;
        }

        double triangle_wave(double amplitude, double frequency, double t) {
            return 2 * amplitude / PI * asin(sin(2 * PI * frequency * t));
        }

        double sawtooth_wave(double amplitude, double frequency, double t) {
            return -2 * amplitude / PI * atan(cos(PI * frequency * t) / sin(PI * frequency * t));
        }

        std::vector<double> eigenv2stdv(Eigen::VectorXd& eigen_vec) {
            std::vector<double> std_vec;
            std_vec.resize(eigen_vec.size());
            Eigen::VectorXd::Map(&std_vec[0], eigen_vec.size()) = eigen_vec;
            return std_vec;
        }

        Eigen::VectorXd stdv2eigenv(std::vector<double>& std_vec) {
            Eigen::Map<Eigen::VectorXd> eigen_vec(&std_vec[0], std_vec.size());
            return eigen_vec;
        }

    }
}



