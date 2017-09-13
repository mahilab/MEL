#pragma once
#include <vector>
#include <Eigen\Dense>
#include <Eigen\StdVector>
#include <limits>
#include <cmath>

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
        // GENERIC FUNCTIONS
        //-------------------------------------------------------------------------

        /// Returns -1 for negative numbers, 1 for positive numbers, and 0 for numbers equal to 0.
        template <typename T> 
        int sign(T val) {
            return (T(0) < val) - (val < T(0));
        }

        /// Clamps value between min and max
        template <typename T>
        T saturate(T value, T max, T min) {
            if (value > max)
                return max;
            else if (value < min)
                return min;
            else
                return value;
        }
        /// Clamps value between -abs_max and +abs_max
        template <typename T>
        T saturate(T value, T abs_max) {
            abs_max = std::abs(abs_max);
            return saturate(value, abs_max, -abs_max);
        }

        /// Returns true if a and b are approximately equal to each other within machine precision
        template <typename T>
        bool approx_equal(T a, T b) {
            return std::abs(a - b) < std::numeric_limits<T>::epsilon();
        }
        /// Returns true if a and b are approximately equal to each other within the specified tolerance
        template <typename T>
        bool approx_equal(T a, T b, T tolerance) {
            return std::abs(a - b) < tolerance;
        }
        
        /// Returns a linearly spaced vector with #n elements between #a and #b. The type returned will
        /// be the same as the type passed in. Be mindful of this; linspace(0,3,3) will return [0,1,3], 
        /// whereas linespace(0.0, 3.0, 3) will return [0.0, 1.5, 3.0]
        template <typename T>
        std::vector<T> linspace(T a, T b, int n) {
            std::vector<T> out(n);
            double delta = static_cast<double>(b - a) / static_cast<double>(n - 1);
            double temp =  static_cast<double>(a);
            out[0] = a;
            for (int i = 1; i < n - 1; i++) {
                temp += delta;
                out[i] = static_cast<T>(temp);
            }
            out[n - 1] = b;
            return out;
        }

        /// Computes a proportial-derivative control effort given gains, reference state, and current state
        double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd);

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

    }

}

