#pragma once
#include <array>
#include "Exo.h"
#include "util.h"
#include "MelShare.h"
#include "Task.h"
#include "Controller.h" 
#include "Clock.h"
#include "Encoder.h"
#include "Motor.h"

class OpenWrist : public mel::Exo {

public:

    struct Config {
        std::array<mel::Daq::Do, 3>      enable_;    // digital output channels that enable motors
        std::array<mel::Daq::Ao, 3>      command_;   // analog output channels that command motors
        std::array<mel::Daq::Encoder, 3> encoder_;   // encoder channels that measure motor positions
        std::array<mel::Daq::EncRate, 3> encrate_;   // encoder channels that measure motor velocities
        std::array<double, 3>            amp_gains_; // motor aplifier gains 
    };

    struct Params {
         
        std::array<double, 3>      kt_                   = {  0.0603,                  0.0603,                  0.0538 };                 // motor torque constants [Nm/A]
        std::array<double, 3>      motor_current_limits_ = {  3.17,                    3.17,                    1.74 };                   // motor current limits [A]
        std::array<double, 3>      motor_torque_limits_  = {  3.17 * 0.0603,           3.17 * 0.0603,           1.74 * 0.0538 };          // motor torque limits [Nm]
        std::array<double, 3>      eta_                  = {  0.4706 / 8.750,          0.4735 / 9.000,          0.2210 / 6.000 };         // transmission ratios [inch/inch]
        std::array<mel::uint32, 3> encoder_res_          = {  500,                     500,                     500 };                    // encoder resolutions [counts/rev]   
        std::array<double, 3>      pos_limits_neg_       = { -86.1123 * mel::DEG2RAD, -63.2490 * mel::DEG2RAD, -36.6765 * mel::DEG2RAD }; // joint position limits in negative rotation [rad]
        std::array<double, 3>      pos_limits_pos_       = { +86.1123 * mel::DEG2RAD, +68.2490 * mel::DEG2RAD, +33.4192 * mel::DEG2RAD }; // joint position limits in positive rotation [rad]
        std::array<double, 3>      vel_limits_           = {  400     * mel::DEG2RAD,  400     * mel::DEG2RAD,  400     * mel::DEG2RAD }; // joint velocity limits [rad/s]
        std::array<double, 3>      joint_torque_limits   = {  4.0,                     4.0,                     3.0,                   }; // joint torque limits [Nm]
        std::array<double, 3>      kin_friction_         = {  0.1891,                  0.0541,                  0.1339 };                 // joint kinetic friction [Nm]

    };

    OpenWrist(Config configuration, Params parameters = Params());
    ~OpenWrist() override;

    ///////////////////
    // PUBLIC FUNCTIONS
    ///////////////////

    double compute_gravity_compensation(mel::uint32 joint);
    double compute_friction_compensation(mel::uint32 joint);
    std::array<double, 3> compute_gravity_compensation();
    std::array<double, 3> compute_friction_compensation();

    void calibrate(mel::Daq* daq);
    void transparency_mode(mel::Daq* daq);

    // OPENWRIST MELSHARE STATE MAP

    mel::share::MelShare state_map_ = mel::share::MelShare("ow_state"); 
    void update_state_map();
    mel::double_vec state_ = mel::double_vec(9, 0);

    const Config config_;
    const Params params_;

    //////////////////////
    // TASKS / CONTROLLERS
    //////////////////////

public:

    class Calibration : public mel::Task {
    public:
        Calibration(OpenWrist* open_wrist, mel::Daq* daq) : Task("ow_calibration"), ow(open_wrist), daq(daq) {}
        OpenWrist* ow;
        mel::Daq* daq;
        std::array<double, 3> kp_gains    = { 50, 40, 40 };
        std::array<double, 3> kd_gains    = { 1.15, 1.0, 0.25 };
        std::array<double, 3> sat_torques = { 2.0, 0.5, 1.0 };
        std::array<double, 3> zeros       = { 0,0,0 };
        std::array<int,    3> dir         = { 1 , 1, -1 };
        mel::uint32 calibrating_joint = 0;
        double pos_ref = 0;
        double vel_ref = 90 * mel::DEG2RAD;
        std::vector<double> stored_positions;
        bool returning = false;
        void start() override;
        void step() override;
        void stop() override;
    };

    class TransparencyMode : public mel::Task {
    public:
        TransparencyMode(OpenWrist* open_wrist, mel::Daq* daq) : Task("ow_transparency_mode"), ow_(open_wrist), daq_(daq) {}
        OpenWrist* ow_;
        mel::Daq* daq_;
        void start() override;
        void step() override;
        void stop() override;
    };

};
