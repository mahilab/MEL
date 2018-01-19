#pragma once
#include <MEL/Core/Exo.hpp>
#include <MEL/Core/Motor.hpp>
#include <MEL/Core/PdController.hpp>
#include <MEL/Exoskeletons/OpenWrist/OwConfiguration.hpp>
#include <MEL/Exoskeletons/OpenWrist/OwParameters.hpp>
#include <array>

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
    void calibrate(bool& stop_flag);

    /// Puts the OpenWrist in an endless graivity and friction compensated state
    void transparency_mode(bool& stop_flag);

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

    std::array<Motor, 3> motors_;

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
