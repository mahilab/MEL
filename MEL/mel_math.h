#pragma once
#include <vector>
#include <Eigen\Dense>
#include <Eigen\StdVector>
#include <limits>
#include "mel_types.h"

namespace mel {

    namespace math {

        //-------------------------------------------------------------------------
        // CONSTANTS
        //-------------------------------------------------------------------------

        const double PI  = 3.14159265358979E+00;                     ///< pi
        const double EPS = std::numeric_limits<double>::epsilon();   ///< smallest double such that 1.0 + EPS != 1.0
        const double INF = std::numeric_limits<double>::infinity();  ///< constant representing positive infinity (negative infinifty is just -INF)

        //-------------------------------------------------------------------------
        // CONVERSION FACTORS
        //-------------------------------------------------------------------------

        const double DEG2RAD = 1.74532925199433E-02;  ///< multiply degrees by this value to convert to radians
        const double RAD2DEG = 5.72957795130824E+01;  ///< multiply radians by this value to convert to degrees
        const double INCH2METER = 0.0254;             ///< multiply inches by this value to convert to meters
        const double METER2INCH = 1.0 / 0.0254;       ///< multiply meters by this value to convert to inches

        //-------------------------------------------------------------------------
        // TRIGONOMETRIC FUNCTIONS
        //-------------------------------------------------------------------------

        //double cos(double angle)  { return std::cos(angle); }
        //double tan(double angle)  { return std::tan(angle); }
        //double asin(double angle) { return std::asin(angle); }
        //double acos(double angle) { return std::acos(angle); }
        //double atan(double angle) { return std::atan(angle); }
        //double atan2(double y, double x) { return std::atan2(y,x); }
        //double sinh(double angle) { return std::sinh(angle); }
        //double cosh(double angle) { return std::cosh(angle); }
        //double tanh(double angle) { return std::tanh(angle); }

        //-------------------------------------------------------------------------
        // GENERIC FUNCTIONS
        //-------------------------------------------------------------------------

        /// Returns -1 for negative numbers, 1 for positive numbers, and 0 for numbers equal to 0.
        template <typename T> 
        int sign(T val) {
            return (T(0) < val) - (val < T(0));
        }

        /// Clamps value between min and max
        double saturate(double value, double max, double min);
        /// Clamps value between -abs_max and +abs_max
        double saturate(double value, double abs_max);

        /// Returns true if a and b are approximately equal to each other within a tolerance (machine precision by default)
        bool approx_equal(double a, double b, double tolerance = EPS);
        
        /// Returns a linearly spaced vector with #n elements between #a and #b. 
        std::vector<double> linspace(double a, double b, uint32 n);

        /// Computes a proportial-derivative control effort given gains, reference state, and current state
        double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd);

        //--------------------------------------------------------------------------
        // STATISTICS
        //--------------------------------------------------------------------------

        /// Returns the absolute value of a vector
        std::vector<double> abs_vec(const std::vector<double>& data);

        /// Returns the sum of a vector of data
        double sum(const std::vector<double>& data);

        /// Returns the mean of a vector of data
        double mean(const std::vector<double>& data);

        /// Returns the population standard deviation of a vector of data
        double stddev_p(const std::vector<double>& data);

        /// Returns the sample standard deviation of a vector of data
        double stddev_s(const std::vector<double>& data);

        //--------------------------------------------------------------------------
        // CYCLIC WAVEFORMS
        //--------------------------------------------------------------------------

        double      sin_wave(double amplitude, double frequency, double t);
        double      cos_wave(double amplitude, double frequency, double t);
        double   square_wave(double amplitude, double frequency, double t);
        double triangle_wave(double amplitude, double frequency, double t);
        double sawtooth_wave(double amplitude, double frequency, double t);

        //-------------------------------------------------------------------------
        // EIGEN RELATED
        //-------------------------------------------------------------------------

        std::vector<double> eigenv2stdv(const Eigen::VectorXd& eigen_vec);

        Eigen::VectorXd stdv2eigenv(std::vector<double>& std_vec);

        double mat_spectral_norm(const Eigen::MatrixXd& mat);

    }

}

