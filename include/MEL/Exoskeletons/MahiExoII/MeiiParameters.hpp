#pragma once 

#include <MEL/Math/Constants.hpp>
#include <MEL/Utility/Types.hpp>
#include <MEL/Utility/Time.hpp>
#include <array>

namespace mel {

    /// Stores the constant parameters associated with the OpenWrist.
    struct MeiiParameters {

        /// Default constructor.
        MeiiParameters() :
            //                    JOINT 0             JOINT 1             JOINT 2             JOINT 3             JOINT 4
            kt_                {  0.127,              0.0603,             0.175,              0.175,              0.175             }, // [Nm/A]
            motor_cont_limits_ {  6.0,                3.17,               0.626,              0.626,              0.626             }, // [A]
            motor_peak_limits_ {  18.0,               18.0,               1.8,                1.8,                1.8               }, // [A]
            motor_i2t_times_   {  seconds(2.0),       seconds(2.0),       seconds(2.0),       seconds(2.0),       seconds(2.0)      }, // [s]
            eta_               {  0.42 / 4.5,         0.0662864,          0.23 * INCH2METER,  0.23 * INCH2METER,  0.23 * INCH2METER }, // [inch/inch] or [m]
            encoder_res_       {  2048,               2048,               2048,               2048,               2048              }, // [counts/rev]
            pos_limits_neg_    { -91.5 * DEG2RAD,    -99 * DEG2RAD,       0.050,              0.050,              0.050             }, // [rad] or [m]
            pos_limits_pos_    {   3.0 * DEG2RAD,     108 * DEG2RAD,      0.133,              0.133,              0.133             }, // [rad] or [m]
            vel_limits_        {   250 * DEG2RAD,     300 * DEG2RAD,      0.4,                0.4,                0.4               }, // [rad/s] or [m/s]
            joint_torque_limits{  10.0,               10.0,               50.0,               50.0,               50.0              }, // [Nm] or [N]
            kin_friction_      {  0.0,                0.0,                0.0,                0.0,                0.0               }  // [Nm] or [N]
        {}

        /// motor torque constants [Nm/A]
        std::array<double, 5> kt_;
        /// motor continous current limits [A]
        std::array<double, 5> motor_cont_limits_;
        /// motor peak current limits [A]
        std::array<double, 5> motor_peak_limits_;
        /// motor i^2*t times [s]
        std::array<Time, 5> motor_i2t_times_;
        /// transmission ratios [inch/inch]
        std::array<double, 5> eta_;
        /// encoder resolutions [counts/rev]
        std::array<uint32, 5> encoder_res_;
        /// joint position limits in negative rotation [rad]
        std::array<double, 5> pos_limits_neg_;
        /// joint position limits in positive rotation [rad]
        std::array<double, 5> pos_limits_pos_;
        /// joint velocity limits [rad/s]
        std::array<double, 5> vel_limits_;
        /// joint torque limits [Nm]
        std::array<double, 5> joint_torque_limits;
        /// joint kinetic friction [Nm]
        std::array<double, 5> kin_friction_;
    };

} // namespace mel


  /*struct Params {
  std::array<double, N_rj_> kt_ = { 0.127, 0.0603, 0.175, 0.175, 0.175 }; ///< motor torque constants [N-m/A]
  std::array<double, N_rj_> motor_hard_current_limits_ = { 8.0, 4.0, 0.75, 0.75, 0.75 }; ///< motor continuous current limits [A]
  std::array<double, N_rj_> motor_continuous_current_limits_ = { 6.0, 3.17, 0.626, 0.626, 0.626 }; ///< motor continuous current limits [A]
  std::array<double, N_rj_> motor_peak_current_limits_ = { 18.0, 18.0, 1.8, 1.8, 1.8 }; ///< motor peak current limits [A]
  std::array<double, N_rj_> motor_i2t_times_ = { 2, 2, 2, 2, 2 }; ///< motor i^2*t times [s]
  std::array<double, N_rj_> eta_ = { 0.42 / 4.5, 0.0662864, INCH2METER*0.23, INCH2METER*0.23, INCH2METER*0.23 }; ///< transmission ratios [inch/inch] or [m]
  std::array<uint32, N_rj_> encoder_res_ = { 2048, 2048, 2048, 2048, 2048 }; ///< encoder resolutions [counts/rev]
  std::array<double, N_rj_> pos_limits_min_ = { -91.5 * DEG2RAD, -99 * DEG2RAD, 0.050, 0.050, 0.050 }; ///< robot joint position limits in negative direction [rad] or [m]
  std::array<double, N_rj_> pos_limits_max_ = { 3 * DEG2RAD, 108 * DEG2RAD, 0.133, 0.133, 0.133 }; ///< robot joint position limits in positive direction [rad] or [m]
  std::array<double, N_rj_> vel_limits_ = { 250 * DEG2RAD, 300 * DEG2RAD, 0.4, 0.4, 0.4 }; ///< robot joint velocity limits [rad/s] or [m/s]
  std::array<double, N_rj_> joint_torque_limits = { 10, 10, 50, 50, 50 }; ///< robot joint torque limits [Nm] or [N]
  std::array<double, N_rj_> amp_gains_ = { 1.8, 1.8, 0.184, 0.184, 0.184 }; ///< motor aplifier gains [A/V]
  };*/
