#include "mel_math.h"
#include <numeric>
#include "mel_util.h"

namespace mel {
    
    namespace math {

        //-------------------------------------------------------------------------
        // GENERIC FUNCTIONS
        //-------------------------------------------------------------------------

        double saturate(double value, double max, double min) {
            if (value > max)
                return max;
            else if (value < min)
                return min;
            else
                return value;
        }

        double saturate(double value, double abs_max) {
            abs_max = std::abs(abs_max);
            return saturate(value, abs_max, -abs_max);
        }

        double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd) {
            return kp * (x_ref - x) + kd * (xd_ref - xd);
        }

        bool approx_equal(double a, double b, double tolerance) {
            return std::abs(a - b) < tolerance;
        }

        std::vector<double> linspace(double a, double b, uint32 n) {
            std::vector<double> out;
            if (n > 1) {
                out.resize(n);
                double delta = (b - a) / (n - 1);
                out[0] = a;
                for (int i = 1; i < n - 1; i++) {
                    out[i] = out[i-1] + delta;
                }
                out[n - 1] = b;
            }
            return out;
        }

        //--------------------------------------------------------------------------
        // STATISTICS
        //--------------------------------------------------------------------------

        double sum(std::vector<double> data) {
            return std::accumulate(data.begin(), data.end(), 0.0);
        }

        double mean(std::vector<double> data) {
            return data.size() > 0 ? sum(data) / data.size() : 0.0;
        }

        double stddev_p(std::vector<double> data) {
            if (data.size() > 0) {
                double avg = mean(data);
                std::vector<double> diff(data.size());
                std::transform(data.begin(), data.end(), diff.begin(), [avg](double x) { return x - avg; });
                double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
                return std::sqrt(sq_sum / data.size());
            }
            else {
                return 0;
            }
        }

        double stddev_s(std::vector<double> data) {
            if (data.size() > 1) {
                double avg = mean(data);
                std::vector<double> diff(data.size());
                std::transform(data.begin(), data.end(), diff.begin(), [avg](double x) { return x - avg; });
                double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
                return std::sqrt(sq_sum / (data.size() - 1));
            }
            else {
                return 0;
            }
        }

        //--------------------------------------------------------------------------
        // CYCLIC WAVEFORMS
        //--------------------------------------------------------------------------

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

        //-------------------------------------------------------------------------
        // EIGEN RELATED
        //-------------------------------------------------------------------------

        std::vector<double> eigenv2stdv(const Eigen::VectorXd& eigen_vec) {
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



