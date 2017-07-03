#pragma once
#include <array>
#include "Exo.h"
#include "util.h"

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
        std::array<double, 3>      eta_            = { 0.468 / 8.750,  0.468 / 9.000,  0.234 / 6.000 }; // transmission ratios    [inch / inch]
        std::array<mel::uint32, 3> encoder_res_    = { 500,            500,            500 };           // encoder resolutions    [counts / rev]
        std::array<double, 3>      kt_             = { 0.0603,         0.0603,         0.0538 };        // motor torque constants [N-m/A]
        std::array<double, 3>      current_limits_ = { 3.17,           3.17,           1.74 };          // motor current limits   [A]
        std::array<double, 3>      kin_friction_   = { 0.1891,         0.0541,         0.1339 };        // joint kinetic friction [N-m]
    };

    OpenWrist(Config configuration, Params parameters = Params());

    const Config config_;
    const Params params_;
};
