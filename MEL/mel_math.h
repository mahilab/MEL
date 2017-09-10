#pragma once
#include <vector>
#include <Eigen\Dense>
#include <Eigen\StdVector>

namespace mel {

    namespace math {

        //-------------------------------------------------------------------------
        // CONSTANTS
        //-------------------------------------------------------------------------

        const double PI      = 3.14159265358979E+00;  ///< constant pi
        const double DEG2RAD = 1.74532925199433E-02;  ///< multiply degrees by this value to convert to radians
        const double RAD2DEG = 5.72957795130824E+01;  ///< multiply radians by this value to convert to degrees

        const double INCH2METER = 0.0254;             ///< multiply inches by this value to convert to meters
        const double METER2INCH = 1.0 / 0.0254;       ///< multiply meters by this value to convert to inches

        //-------------------------------------------------------------------------
        // GENERIC FUNCTIONS
        //-------------------------------------------------------------------------

        /// Returns -1 for negative numbers, 1 for positive numbers, and 0 for numbers equal to 0.
        template <typename T> int sign(T val) {
            return (T(0) < val) - (val < T(0));
        }

        double pd_controller(double kp, double kd, double x_ref, double x, double xd_ref, double xd);
        double saturate(double value, double max, double min);
        double saturate(double value, double abs_max);

        //--------------------------------------------------------------------------
        // WAVEFORMS
        //--------------------------------------------------------------------------

        double sin_wave(double amplitude, double frequency, double t);
        double cos_wave(double amplitude, double frequency, double t);
        double square_wave(double amplitude, double frequency, double t);
        double triangle_wave(double amplitude, double frequency, double t);
        double sawtooth_wave(double amplitude, double frequency, double t);

        //-------------------------------------------------------------------------
        // EIGEN RELATED
        //-------------------------------------------------------------------------

        std::vector<double> eigenv2stdv(Eigen::VectorXd& eigen_vec);

        Eigen::VectorXd stdv2eigenv(std::vector<double>& std_vec);

    }

}

