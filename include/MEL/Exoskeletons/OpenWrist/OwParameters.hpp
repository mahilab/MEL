#pragma once

#include <MEL/Math/Constants.hpp>
#include <MEL/Utility/Types.hpp>
#include <MEL/Utility/Time.hpp>
#include <array>

namespace mel {

/// Stores the constant parameters associated with the OpenWrist.
struct OwParameters {

    /// Default constructor.
    OwParameters() :
        //                    JOINT 0             JOINT 1             JOINT 2
        kt_                {  0.0603,             0.0603,             0.0538            }, // [Nm/A]
        motor_cont_limits_ {  3.17,               3.17,               1.74              }, // [A]
        motor_peak_limits_ {  4.0,                4.0,                4.0               }, // [A]
        motor_i2t_times_   {  seconds(2),         seconds(2),         seconds(2)        }, // [s]
        eta_               {  0.4706 / 8.750,     0.4735 / 9.000,     0.2210 / 6.000    }, // [inch/inch]
        encoder_res_       {  500,                500,                500               }, // [counts/rev]
        pos_limits_neg_    { -86.1123 * DEG2RAD, -63.2490 * DEG2RAD, -42.0321 * DEG2RAD }, // [rad]
        pos_limits_pos_    { +86.1123 * DEG2RAD, +68.2490 * DEG2RAD, +30.9087 * DEG2RAD }, // [rad]
        vel_limits_        {  400     * DEG2RAD,  400     * DEG2RAD,  400     * DEG2RAD }, // [rad/s]
        joint_torque_limits{  10.0,               10.0,               10.0,             }, // [Nm]
        kin_friction_      {  0.1891,             0.0541,             0.1339            }  // [Nm]
    {}

    /// motor torque constants [Nm/A]
    std::array<double, 3> kt_;
    /// motor continous current limits [A]
    std::array<double, 3> motor_cont_limits_;
    /// motor peak current limits [A]
    std::array<double, 3> motor_peak_limits_;
    /// motor i^2*t times [s]
    std::array<Time, 3> motor_i2t_times_;
    /// transmission ratios [inch/inch]
    std::array<double, 3> eta_;
    /// encoder resolutions [counts/rev]
    std::array<uint32, 3> encoder_res_;
    /// joint position limits in negative rotation [rad]
    std::array<double, 3> pos_limits_neg_;
    /// joint position limits in positive rotation [rad]
    std::array<double, 3> pos_limits_pos_;
    /// joint velocity limits [rad/s]
    std::array<double, 3> vel_limits_;
    /// joint torque limits [Nm]
    std::array<double, 3> joint_torque_limits;
    /// joint kinetic friction [Nm]
    std::array<double, 3> kin_friction_;
};

} // namespace mel
