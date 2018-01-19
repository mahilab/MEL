#pragma once
#include <complex>
#include <vector>
//#include <Eigen\Dense>
//#include <Eigen\StdVector>

namespace mel {

//==============================================================================
// TRIGONOMETRIC FUNCTIONS
//==============================================================================

extern double sin(double radians);
extern double cos(double radians);
extern double tan(double radians);
extern double asin(double radians);
extern double acos(double radians);
extern double atan(double radians);
extern double atan2(double y, double x);
extern double sinh(double radians);
extern double cosh(double radians);
extern double tanh(double radians);

//==============================================================================
// GENERIC FUNCTIONS
//==============================================================================

/// Returns -1 for negative numbers, 1 for positive numbers, and 0 for numbers equal to 0.
template <typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

/// Computes the absolute of a value
extern double abs(double value);

/// Returns minimum of two values
extern double min(double a, double b);

/// Returns minimum value in a a vector
extern double min(const std::vector<double>& values);

/// Returns maximum of two values
extern double max(double a, double b);

/// Returns maximum value in a a vector
extern double max(const std::vector<double>& values);

/// Computes the square root a number
extern double sqrt(double value);

/// Computes the square of a number efficicently
extern double sq(double value);

/// Computes the power of a base and exponent
extern double pow(double base, double exp);

/// Clamps value between min and max
extern double saturate(double value, double min, double max);

/// Clamps value between -abs_max and +abs_max
extern double saturate(double value, double abs_max);

/// Returns true if a and b are approximately equal to each other within a tolerance (machine precision by default)
extern bool approx_equal(double a, double b, double tolerance);

/// Returns a linearly spaced vector with #n elements between #a and #b.
extern std::vector<double> linspace(double a, double b, int n);

/// Computes a proportional-derivative control effort given gains, reference state, and current state
extern double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd);

/// Logistic sigmoid
extern double sigmoid(double a);

/// Computes the automatic derivative of a function.
extern double auto_diff(std::complex<double> (*f)(std::complex<double>), double x);

//==============================================================================
// STATISTICS
//==============================================================================

/// Returns the absolute value of a vector
extern std::vector<double> abs_vec(const std::vector<double>& data);

/// Returns the sum of a vector of data
extern double sum(const std::vector<double>& data);

/// Returns the mean of a vector of data
extern double mean(const std::vector<double>& data);

/// Returns the population standard deviation of a vector of data
extern double stddev_p(const std::vector<double>& data);

/// Returns the sample standard deviation of a vector of data
extern double stddev_s(const std::vector<double>& data);

//==============================================================================
// EIGEN RELATED
//==============================================================================

/*

void eigvec_to_stdvec(const Eigen::VectorXd& eigen_vec, std::vector<double>& std_vec);

void stdvec_to_eigvec(std::vector<double>& std_vec, Eigen::VectorXd& eigen_vec);

std::vector<double> copy_eigvec_to_stdvec(const Eigen::VectorXd& eigen_vec);

Eigen::VectorXd copy_stdvec_to_eigvec(const std::vector<double>& std_vec);

std::vector<std::vector<double>> copy_eigmat_to_stdvecvec(const Eigen::MatrixXd& eigen_mat);

Eigen::MatrixXd copy_stdvecvec_to_eigmat(const std::vector<std::vector<double>>& std_vecvec);

double mat_spectral_norm(const Eigen::MatrixXd& mat);

/// returns the value of the softmax function for element k of input vector a
double softmax(const Eigen::VectorXd& a, int k);

*/

} // namespace mel

