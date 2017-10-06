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
                for (uint32 i = 1; i < n - 1; i++) {
                    out[i] = out[i-1] + delta;
                }
                out[n - 1] = b;
            }
            return out;
        }

        //--------------------------------------------------------------------------
        // STATISTICS
        //--------------------------------------------------------------------------

        std::vector<double> abs_vec(const std::vector<double>& data) {
            std::vector<double> abs_data(data.size());
            for (int i = 0; i < data.size(); ++i) {
                abs_data[i] = std::abs(data[i]);
            }
            return abs_data;
        }        

        double sum(const std::vector<double>& data) {
            return std::accumulate(data.begin(), data.end(), 0.0);
        }

        double mean(const std::vector<double>& data) {
            return data.size() > 0 ? sum(data) / data.size() : 0.0;
        }

        double stddev_p(const std::vector<double>& data) {
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

        double stddev_s(const std::vector<double>& data) {
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

        std::vector<double> eigvec_to_stdvec(const Eigen::VectorXd& eigen_vec) {
            std::vector<double> std_vec;
            std_vec.resize(eigen_vec.size());
            Eigen::VectorXd::Map(&std_vec[0], eigen_vec.size()) = eigen_vec;
            return std_vec;
        }

        Eigen::VectorXd stdvec_to_eigvec(std::vector<double>& std_vec) {
            Eigen::Map<Eigen::VectorXd> eigen_vec(&std_vec[0], std_vec.size());
            return eigen_vec;
        }

        std::vector<std::vector<double>> eigmat_to_stdvecvec(const Eigen::MatrixXd& eigen_mat) {
            std::vector<std::vector<double>> std_vecvec;
            for (int i = 0; i < eigen_mat.rows(); ++i) {
                std_vecvec.push_back(eigvec_to_stdvec(eigen_mat.row(i)));
            }
            return std_vecvec;
        }
        
        Eigen::MatrixXd stdvecvec_to_eigmat(std::vector<std::vector<double>>& std_vecvec) {
            int cols = std_vecvec[0].size();
            Eigen::MatrixXd eigmat(std_vecvec.size(), cols);
            for (int i = 0; i < std_vecvec.size(); ++i) {
                if (std_vecvec[i].size() == cols) {
                    eigmat.row(i) = stdvec_to_eigvec(std_vecvec[i]);
                }
                else {
                    std::cout << "ERROR: Input must have same number of cols in each row to be converted into Eigen Matrix type." << std::endl;
                }
            }
            return eigmat;
        }

        double mat_spectral_norm(const Eigen::MatrixXd& mat) {
            Eigen::EigenSolver<Eigen::MatrixXd> eigensolver(mat.transpose() * mat, false);
            if (eigensolver.info() != Eigen::Success) {
                util::print("ERROR: Eigensolver did not converge in mat_spectral_norm");
                return 0;
            }
            Eigen::EigenSolver<Eigen::MatrixXd>::EigenvalueType lambda = eigensolver.eigenvalues();
            double_vec lambda_abs(lambda.size(), 0.0);
            for (int i = 0; i < lambda.size(); ++i) {
                lambda_abs[i] = std::abs(lambda[i]);
            }
            std::vector<double>::iterator lambda_max;
            lambda_max = std::max_element(lambda_abs.begin(), lambda_abs.end());
            return std::sqrt(*lambda_max);
        }

    }
}



