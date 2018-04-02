#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace mel {

//==============================================================================
// TRIGONOMETRIC FUNCTIONS
//==============================================================================

double sin(double radians) {
    return std::sin(radians);
}

double cos(double radians)  {
    return std::cos(radians);
}

double tan(double radians) {
    return std::tan(radians);
     }
double asin(double radians) {
    return std::asin(radians);
}

double acos(double radians) {
    return std::acos(radians);
}

double atan(double radians) {
    return std::atan(radians);
}

double atan2(double y, double x) {
    return std::atan2(y,x);
}

double sinh(double radians) {
    return std::sinh(radians);
}

double cosh(double radians) {
    return std::cosh(radians);
}

double tanh(double radians) {
    return std::tanh(radians);
}

//==============================================================================
// GENERIC FUNCTIONS
//==============================================================================

double abs(double value) {
    return std::abs(value);
}

double min(double a, double b) {
    return std::min(a, b);
}

double min(const std::vector<double>& values) {
    return *min_element(values.begin(), values.end());
}

double max(double a, double b) {
    return std::max(a, b);
}

double max(const std::vector<double>& values) {
    return *max_element(values.begin(), values.end());
}

double sqrt(double value) {
    return std::sqrt(value);
}

 double sq(double value) {
    return value*value;
 }

double pow(double base, double exp){
    return std::pow(base, exp);
}

double saturate(double value, double min, double max) {
    if (value > max)
        return max;
    else if (value < min)
        return min;
    else
        return value;
}

double saturate(double value, double abs_max) {
    abs_max = std::abs(abs_max);
    return saturate(value, -abs_max, abs_max);
}

double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd) {
    return kp * (x_ref - x) + kd * (xd_ref - xd);
}

bool approx_equal(double a, double b, double tolerance) {
    return std::abs(a - b) < tolerance;
}

std::vector<double> linspace(double a, double b, int n) {
    std::vector<double> out;
    if (n > 1) {
        out.resize(n);
        double delta = (b - a) / (n - 1);
        out[0] = a;
        for (int i = 1; i < n - 1; i++) {
            out[i] = out[i - 1] + delta;
        }
        out[n - 1] = b;
    }
    return out;
}

double sigmoid(double a) {
    double b = std::exp(-a);
    if (std::isinf(b)) {
        return 0;
    }
    else {
        return 1.0 / (1.0 + b);
    }
}

double auto_diff(std::complex<double>(*f)(std::complex<double>), double x) {
    return f(std::complex<double>(x, 1.0e-22)).imag() / 1.0e-22;
}

//==============================================================================
// STATISTICS
//==============================================================================

std::vector<double> abs_vec(const std::vector<double>& data) {
    std::vector<double> abs_data(data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
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

extern std::vector<double> linear_regression(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size()) {
        LOG(Error) << "Length of x (" << x.size() << ") not equal to length of y (" << y.size() << ")";
        return std::vector<double>();
    }

    double xbar = mean(x);
    double ybar = mean(y);
    double xbar2 = xbar * xbar;
    double xbarybar = xbar * ybar;

    std::size_t n = x.size();
    std::vector<double> x2_xbar2(n);
    std::vector<double> xy_xbarybar(n);

    for (std::size_t i = 0; i < n; ++i) {
        x2_xbar2[i]    = x[i] * x[i] - xbar2;
        xy_xbarybar[i] = x[i] * y[i] - xbarybar;
    }

    double sigmax2  = mean(x2_xbar2);
    double sigmaxy = mean(xy_xbarybar);

    double m = sigmaxy / sigmax2;
    double b = -xbar * m + ybar;

    return std::vector<double>{m, b};
}

extern void gauss_mlt_params(const std::vector<std::vector<double>>& sample_data, std::vector<double>& sample_mean, std::vector<std::vector<double>>& sample_cov) {
    std::size_t N = sample_data.size();
    std::size_t sample_dim;
    if (N > 0) {
        sample_dim = sample_data[0].size();
        for (std::size_t i = 0; i < N; ++i) {
            if (sample_data[i].size() != sample_dim) {
                LOG(Warning) << "Data given to gauss_mlt_params() contains samples of different sizes. Parameters were not computed.";
                return;
            }
        }
    }
    else {
        LOG(Warning) << "Data given to gauss_mlt_params() was empty. Parameters were not computed.";
        return;
    }   

    sample_mean = std::vector<double>(sample_dim, 0.0);
    sample_cov = std::vector<std::vector<double>>(sample_dim, std::vector<double>(sample_dim, 0.0));

    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < sample_dim; ++j) {
            sample_mean[j] += sample_data[i][j];
        }
    }
    for (std::size_t i = 0; i < sample_dim; ++i) {
        sample_mean[i] /= N;
    }

    std::vector<double> sample(sample_dim, 0.0);
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < sample_dim; ++j) {
            sample[j] = sample_data[i][j] - sample_mean[j];
        }
        for (std::size_t j = 0; j < sample_dim; ++j) {
            for (std::size_t k = 0; k < sample_dim; ++k) {
                sample_cov[j][k] += sample[j] * sample[k];
            }
        }
    }
    double sample_cov_scalar = N - 1;
    for (std::size_t i = 0; i < sample_dim; ++i) {
        for (std::size_t j = 0; j < sample_dim; ++j) {
            sample_cov[i][j] /= sample_cov_scalar;
        }
    }
    
    /*std::size_t n_rows = sample_data.size();
    std::size_t n_cols = sample_data[0].size();

    sample_mean = std::vector<double>(n_rows, 0.0);
    sample_cov = std::vector<std::vector<double>>(n_rows);

    for (std::size_t i = 0; i < n_rows; ++i) {
        if (sample_data[i].size() != n_cols) {
            LOG(Error) << "Function gauss_mlt_params was given argument sample_data with inconsistent column lengths.";
            return;
        }
        sample_cov[i] = std::vector<double>(n_rows, 0.0);
    }

    for (std::size_t i = 0; i < n_rows; ++i) {
        sample_mean[i] = mean(sample_data[i]);
    }

    std::vector<double> sample(n_rows);
    for (std::size_t j = 0; j < n_cols; ++j) {
        for (std::size_t i = 0; i < n_rows; ++i) {
            sample[i] = sample_data[i][j] - sample_mean[i];          
        }
        for (std::size_t i = 0; i < n_rows; ++i) {
            for (std::size_t k = 0; k < n_rows; ++k) {
                sample_cov[i][k] += sample[i] * sample[k] / static_cast<double>(n_cols-1);
            }
        }
    }*/
}

//==============================================================================
// SIGNAL PROCESSING
//==============================================================================

//extern std::vector<double> downsample(const std::vector<double>& signal, uint32 ds_factor) {
//    if (signal.empty() || ds_factor == 0 || ds_factor == 1) {
//        return signal;
//    }
//
//    std::vector<double> ds_signal(((signal.size() - 1) / ds_factor) + 1);
//    for (std::size_t i = 0; i < ds_signal.size(); ++i) {
//        ds_signal[i] = signal[i * ds_factor];
//    }
//
//    return ds_signal;
//}
//
//extern std::vector<std::vector<double>> bin_signal(const std::vector<double>& signal, std::size_t bin_size) {
//    std::vector<std::vector<double>> signal_bins = { signal };
//    if (signal.size() < bin_size) {
//        LOG(Warning) << "Signal was not long enough to split into requested bin size.";
//        return signal_bins;
//    }
//    if (bin_size == 0 || bin_size == 1) {
//        return signal_bins;
//    }
//    std::size_t bin_count = signal.size() / bin_size;
//    signal_bins.resize(bin_count);
//    std::size_t sample_count = (signal.size() / bin_size) * bin_size;
//    for (std::size_t i = 0; i < sample_count; ++i) {
//        if (i % bin_size == 0) {
//            signal_bins[i / bin_size].resize(bin_size);
//        }
//        signal_bins[i / bin_size][i % bin_size] = signal[i];
//    }
//    return signal_bins;
//}



//==============================================================================
// EIGEN RELATED
//==============================================================================

#ifdef MEL_INCLUDE_EIGEN

void eigvec_to_stdvec(const Eigen::VectorXd& eigen_vec, std::vector<double>& std_vec) {
    std_vec.resize(eigen_vec.size());
    Eigen::VectorXd::Map(&std_vec[0], eigen_vec.size()) = eigen_vec;
}

void stdvec_to_eigvec(std::vector<double>& std_vec, Eigen::VectorXd& eigen_vec) {
    eigen_vec = Eigen::Map<Eigen::VectorXd>(std_vec.data(), std_vec.size());
}

std::vector<double> copy_eigvec_to_stdvec(const Eigen::VectorXd& eigen_vec) {
    std::vector<double> std_vec(eigen_vec.size());
    for (int i = 0; i < eigen_vec.size(); ++i) {
        std_vec[i] = eigen_vec[i];
    }
    return std_vec;
}

Eigen::VectorXd copy_stdvec_to_eigvec(const std::vector<double>& std_vec) {
    Eigen::VectorXd eigen_vec(std_vec.size());
    for (int i = 0; i < std_vec.size(); ++i) {
        eigen_vec[i] = std_vec[i];
    }
    return eigen_vec;
}

std::vector<std::vector<double>> copy_eigmat_to_stdvecvec(const Eigen::MatrixXd& eigen_mat) {
    std::vector<std::vector<double>> std_vecvec(eigen_mat.rows());
    for (int i = 0; i < eigen_mat.rows(); ++i) {
        std_vecvec[i] = std::vector<double>(eigen_mat.cols());
        for (int j = 0; j < eigen_mat.cols(); ++j) {
            std_vecvec[i][j] = eigen_mat(i, j);
        }
    }
    return std_vecvec;
}

Eigen::MatrixXd copy_stdvecvec_to_eigmat(const std::vector<std::vector<double>>& std_vecvec) {
    size_t cols = std_vecvec[0].size();
    Eigen::MatrixXd eigen_mat(std_vecvec.size(), cols);
    for (int i = 0; i < std_vecvec.size(); ++i) {
        if (std_vecvec[i].size() == cols) {
            for (int j = 0; j < cols; ++j) {
                eigen_mat(i, j) = std_vecvec[i][j];
            }
        }
        else {
            LOG(Error) << "Input must have same number of cols in each row to be converted into Eigen Matrix type";
        }
    }
    return eigen_mat;
}

double mat_spectral_norm(const Eigen::MatrixXd& mat) {
    Eigen::EigenSolver<Eigen::MatrixXd> eigensolver(mat.transpose() * mat, false);
    if (eigensolver.info() != Eigen::Success) {
        print("ERROR: Eigensolver did not converge in mat_spectral_norm");
        return 0;
    }
    Eigen::EigenSolver<Eigen::MatrixXd>::EigenvalueType lambda = eigensolver.eigenvalues();
    std::vector<double> lambda_abs(lambda.size(), 0.0);
    for (int i = 0; i < lambda.size(); ++i) {
        lambda_abs[i] = std::abs(lambda[i]);
    }
    std::vector<double>::iterator lambda_max;
    lambda_max = std::max_element(lambda_abs.begin(), lambda_abs.end());
    return std::sqrt(*lambda_max);
}

//void bin_linear_discriminant_model(const std::vector<std::vector<double>>& class_0_data, const std::vector<std::vector<double>>& class_1_data, std::vector<double>& w, double& w_0) {
//
//    // data dimension
//    std::size_t D = class_0_data.size();
//    if (class_1_data.size() != D) {
//        LOG(Error) << "Data given to bin_linear_discriminant_model was not of the same size.";
//        return;
//    }
//    Eigen::MatrixXd sample_cov = Eigen::MatrixXd::Zero(D, D);
//    Eigen::MatrixXd sample_cov_inv(D, D);
//    Eigen::VectorXd w_eig(D);
//    Eigen::VectorXd w_0_eig(1);
//    w = std::vector<double>(D);
//
//    double N_0 = (double) class_0_data[0].size();
//    std::vector<double> class_0_sample_mean;
//    std::vector<std::vector<double>> class_0_sample_cov;
//    gauss_mlt_params(class_0_data, class_0_sample_mean, class_0_sample_cov);
//    Eigen::VectorXd class_0_sample_mean_eig = copy_stdvec_to_eigvec(class_0_sample_mean);
//    Eigen::MatrixXd class_0_sample_cov_eig = copy_stdvecvec_to_eigmat(class_0_sample_cov);
//
//    double N_1 = (double) class_1_data[0].size();
//    std::vector<double> class_1_sample_mean;
//    std::vector<std::vector<double>> class_1_sample_cov;
//    gauss_mlt_params(class_1_data, class_1_sample_mean, class_1_sample_cov);
//    Eigen::VectorXd class_1_sample_mean_eig = copy_stdvec_to_eigvec(class_1_sample_mean);
//    Eigen::MatrixXd class_1_sample_cov_eig = copy_stdvecvec_to_eigmat(class_1_sample_cov);
//
//    sample_cov = (N_0 / (N_0 + N_1)) * class_0_sample_cov_eig + (N_1 / (N_0 + N_1)) * class_1_sample_cov_eig;
//    if (sample_cov.fullPivLu().isInvertible())
//        sample_cov_inv = sample_cov.fullPivLu().inverse();
//    else {
//        LOG(Error) << "Sample covariance matrix cannot be inverted.";
//        return;
//    }
//
//    // compute linear model
//    w_eig = sample_cov_inv * (class_1_sample_mean_eig - class_0_sample_mean_eig);
//    w_0_eig = -0.5 * class_1_sample_mean_eig.transpose() * sample_cov_inv * class_1_sample_mean_eig + 0.5 * class_0_sample_mean_eig.transpose() * sample_cov_inv * class_0_sample_mean_eig;
//    w = copy_eigvec_to_stdvec(w_eig);
//    w_0 = w_0_eig[0];
//}

//double softmax(const Eigen::VectorXd& a, int k) {
//    if (k < 0 || k > a.size()) {
//        print("ERROR: Function softmax received input index k outside of bounds of input vector a.");
//        return NAN;
//    }
//    Eigen::VectorXd b(a.size());
//    Eigen::VectorXd c = Eigen::VectorXd::Constant(a.size(), a.minCoeff());
//    b = a - c;
//
//    for (int i = 0; i < b.size(); ++i) {
//        b(i) = std::exp(b(i));
//    }
//    if (b.allFinite() && b.sum() != 0) {
//        return b(k) / b.sum();
//    }
//    else {
//        if (a(k) == a.maxCoeff()) {
//            return 1.0;
//        }
//        else {
//            return 0.0;
//        }
//    }
//}

#endif // MEL_INCLUDE_EIGEN

} // namespace mel



