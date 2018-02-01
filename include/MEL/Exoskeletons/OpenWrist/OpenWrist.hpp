// MIT License
//
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

#ifndef MEL_OPENWRIST_HPP
#define MEL_OPENWRIST_HPP

#include <MEL/Core/Exo.hpp>
#include <MEL/Core/Motor.hpp>
#include <MEL/Core/PdController.hpp>
#include <MEL/Exoskeletons/OpenWrist/OwConfiguration.hpp>
#include <MEL/Exoskeletons/OpenWrist/OwParameters.hpp>
#include <atomic>
#include <vector>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class OpenWrist : public Exo {

public:

    /// Default constructor
    OpenWrist(OwConfiguration configuration, OwParameters parameters = OwParameters());

    /// Default destructor
    ~OpenWrist() override;

    /// Computes a model based graivty compensation torque for a specific joint
    /// given the current positional configuration of the OpenWrist
    double compute_gravity_compensation(uint32 joint);

    /// Computes a tanh based friction compensation torque for a specific joint
    /// given the current velocity and experimental kinetic friction esimates
    double compute_friction_compensation(uint32 joint);

    /// Computes gravity compensation for all joints
    std::array<double, 3> compute_gravity_compensation();

    /// computes friction compensatio for all joints
    std::array<double, 3> compute_friction_compensation();

    /// Calibrates each joint position in sequence and zeros the encoders
    void calibrate(std::atomic<bool>& stop_flag);

    /// Puts the OpenWrist in an endless graivity and friction compensated state
    void transparency_mode(std::atomic<bool>& stop_flag);

public:

    OwConfiguration config_;    ///< this OpenWrist's Config, set during construction
    const OwParameters params_; ///< this OpenWrist's Params, set during construction

    /// critically damped PD controllers for each joint
    std::array<PdController, 3> pd_controllers_ = std::array<PdController, 3>{
        PdController(25, 1.15), // joint 0 ( Nm/rad , Nm-s/rad )
        PdController(20, 1.00), // joint 1 ( Nm/rad , Nm-s/rad )
        PdController(20, 0.25)  // joint 2 ( Nm/rad , Nm-s/rad )
    };

public:

    std::vector<Motor> motors_;

};

} // namespace mel

#endif // MEL_OPENWRIST_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
