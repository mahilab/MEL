#pragma once
#include <MEL/Core/Exo.hpp>
#include <MEL/Core/Daq.hpp>
#include <MEL/Core/PdController.hpp>
#include <MEL/Math/Constants.hpp>

#if _WIN32
#include <MEL/Communications/MelShare.hpp>
#endif
#include <array>

namespace mel {

class OpenWrist : public Exo {

public:

    //-------------------------------------------------------------------------
    // CONFIGURATION / PARAMETERS STRUCTS
    //-------------------------------------------------------------------------

    /// Used to configure which DAQ channels are associated with each OpenWrist
    /// joint for enabling motors, commanding/sensing currents, and reading
    /// encoders. Amplifer gains are also specificed through this struct.
    struct Config {
        std::array<Daq::Do, 3>      enable_;    ///< digital output channels that enable motors
        std::array<Daq::Ao, 3>      command_;   ///< analog output channels that command motor currents
        std::array<Daq::Ai, 3>      sense_;     ///< analog input channels that sense motor current
        std::array<Daq::Encoder, 3> encoder_;   ///< encoder channels that measure motor positions
        std::array<Daq::EncRate, 3> encrate_;   ///< encoder channels that measure motor velocities
        std::array<double, 3>             amp_gains_; ///, motor aplifier gains
    };

    /// Stores the constant parameters associated with the OpenWrist. The
    /// default values may be replaced by passing a custom Params struct
    /// to the OpenWrist constructor. Unless physical hardware changes
    /// have been made to the OpenWrist, there should be no reason to
    /// modify these values.
    struct Params {
        Params() :
            kt_{ 0.0603, 0.0603, 0.0538 },
            motor_cont_limits_{ 3.17, 3.17, 1.74 },
            motor_peak_limits_{ 4.0, 4.0, 4.0 },
            motor_i2t_times_{ 2.0, 2.0, 2.0 },
            eta_{ 0.4706 / 8.750, 0.4735 / 9.000, 0.2210 / 6.000 },
            encoder_res_{ 500, 500, 500 },
            pos_limits_neg_{ -86.1123 * DEG2RAD, -63.2490 * DEG2RAD, -42.0321 * DEG2RAD },
            pos_limits_pos_{ +86.1123 * DEG2RAD, +68.2490 * DEG2RAD, +30.9087 * DEG2RAD },
            vel_limits_{ 400 * DEG2RAD, 400 * DEG2RAD, 400 * DEG2RAD },
            joint_torque_limits{ 10.0, 10.0, 10.0, },
            kin_friction_{ 0.1891, 0.0541, 0.1339 }
        {}

        /// motor torque constants [Nm/A]
        std::array<double, 3> kt_;
        /// motor continous current limits [A]
        std::array<double, 3> motor_cont_limits_;
        /// motor peak current limits [A]
        std::array<double, 3> motor_peak_limits_;
        /// motor i^2*t times [s]
        std::array<double, 3> motor_i2t_times_;
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

    //-------------------------------------------------------------------------
    // CONSTRUCTOR / DESTRUCTOR
    //-------------------------------------------------------------------------

    /// Default constructor
    OpenWrist(Config configuration, Params parameters = Params());
    /// Default destructor
    ~OpenWrist() override;

    //-------------------------------------------------------------------------
    // PUBLIC FUNCTIONS
    //-------------------------------------------------------------------------

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
    void calibrate();
    /// Puts the OpenWrist in an endless graivity and friction compensated state
    void transparency_mode();

    #ifdef _WIN32
    /// Updates the OpenWrist's state MELShare map
    void update_state_map();
    #endif

    //-------------------------------------------------------------------------
    // PUBLIC VARIABLES
    //-------------------------------------------------------------------------

    #ifdef _WIN32
    MelShare state_map_;

    std::vector<double> state_ = std::vector<double>(15, 0);
    #endif

    const Config config_; ///< this OpenWrist's Config, set during construction
    const Params params_; ///< this OpenWrist's Params, set during construction

    std::vector<Daq*> daqs_; ///< all unique DAQs found in config_

    /// critically damped PD controllers for each joint
    std::array<PdController, 3> pd_controllers_ = std::array<PdController, 3>{
        PdController(25, 1.15), // joint 0 ( Nm/rad , Nm-s/rad )
        PdController(20, 1.00), // joint 1 ( Nm/rad , Nm-s/rad )
        PdController(20, 0.25)  // joint 2 ( Nm/rad , Nm-s/rad )
    };

protected:

    //-------------------------------------------------------------------------
    // PROTECTED VARIABLES
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // PROTECTED FUNCTIONS
    //-------------------------------------------------------------------------

    /// Adds a Daq to daqs_ if it's not already in there based on the Device
    /// variable name_
    void add_daq(Daq* daq);

};

} // namespace mel
