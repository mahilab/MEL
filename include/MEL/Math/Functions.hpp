// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_FUNCTIONS_HPP
#define MEL_FUNCTIONS_HPP

#include <complex>
#include <vector>
#ifdef MEL_INCLUDE_EIGEN
    #include <Eigen\Dense>
    #include <Eigen\StdVector>
#endif

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

/// Computes a linear regression slope and intercept {m, b} for y = m*x + b
extern std::vector<double> linear_regression(const std::vector<double>& x, const std::vector<double>& y);

//==============================================================================
// EIGEN RELATED
//==============================================================================

#ifdef MEL_INCLUDE_EIGEN

void eigvec_to_stdvec(const Eigen::VectorXd& eigen_vec, std::vector<double>& std_vec);

void stdvec_to_eigvec(std::vector<double>& std_vec, Eigen::VectorXd& eigen_vec);

std::vector<double> copy_eigvec_to_stdvec(const Eigen::VectorXd& eigen_vec);

Eigen::VectorXd copy_stdvec_to_eigvec(const std::vector<double>& std_vec);

std::vector<std::vector<double>> copy_eigmat_to_stdvecvec(const Eigen::MatrixXd& eigen_mat);

Eigen::MatrixXd copy_stdvecvec_to_eigmat(const std::vector<std::vector<double>>& std_vecvec);

double mat_spectral_norm(const Eigen::MatrixXd& mat);

/// returns the value of the softmax function for element k of input vector a
 double softmax(const Eigen::VectorXd& a, int k);

#endif

} // namespace mel

#endif // MEL_FUNCTIONS_HPP
