// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Mechatronics/Actuator.hpp>
#include <MEL/Mechatronics/PositionSensor.hpp>
#include <MEL/Mechatronics/VelocitySensor.hpp>
#include <array>
#include <memory>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Joint : public Device {
public:
    /// Constructor
    Joint(const std::string& name,
          Actuator* actuator,
          double actuator_transmission,
          PositionSensor* position_sensor,
          double position_sensor_transmission,
          VelocitySensor* velocity_sensor,
          double velocity_sensor_transmission,
          std::array<double, 2> position_limits = { -INF, INF },
          double velocity_limit = INF,
          double torque_limit = INF,
          bool saturate = true);

    /// Constructor
    Joint(const std::string& name,
        Actuator* actuator,
        PositionSensor* position_sensor,
        VelocitySensor* velocity_sensor,
        double transmission,
        std::array<double, 2> position_limits = { -INF, INF },
        double velocity_limit = INF,
        double torque_limit = INF,
        bool saturate = true);

    /// Converts PositionSensor position to Joint position
    double get_position();

    /// Converts PositionSensor velocity to Joint velocity
    double get_velocity();

    /// Returns the currently set joint torque
    double get_torque_command();

    /// Returns the sensed joint torque if the Actuator supports sensing
    double get_torque_sense();

    /// Sets the joint torque to #new_torque
    void set_torque(double new_torque);

    /// Adds #additional_torque to the currently set torque
    void add_torque(double additional_torque);

    /// Gets current position, checks it against limits, and returns true if min
    /// or max exceeded, false otherwise
    bool position_limit_exceeded();

    /// Gets current velocity, checks it against limit, and returns true if
    /// exceeded, false otherwise
    bool velocity_limit_exceeded();

    ///  Gets last commanded torque, checks it against torque limit, and returns
    ///  true if exceeded, false otherise
    bool torque_limit_exceeded();

    /// Gets current position, velocity, and torque, checks them against limits,
    /// and returns true if either exceeded, false otherwise
    bool any_limit_exceeded();

    /// Gets the Joint Actuator
    template <class T = Actuator>
    T* get_actuator() {
        return static_cast<T*>(actuator_);
    }

    /// Gets the Joint PositionSensor
    template <class T = PositionSensor>
    T* get_position_sensor() {
        return static_cast<T*>(position_sensor_);
    }

    /// Gets the Joint VelocitySensor
    template <class T = VelocitySensor>
    T* get_velocity_sensor() {
        return static_cast<T*>(velocity_sensor_);
    }

protected:

    /// Enables the joint's position sensor, velocity sensor, and actuator
    bool on_enable() override;

    /// Disables the joint's position sensor, velocity sensor, and actuator
    bool on_disable() override;

protected:
    Actuator* actuator_;               ///< pointer to the Actuator of this Joint
    PositionSensor* position_sensor_;  ///< pointer to the PositionSensor of this Joint
    VelocitySensor* velocity_sensor_;  ///< pointer to the VelocitySensor of this Joint

    double actuator_transmission_;          ///< transmission ratio describing the
                                            ///< multiplicative gain in torque from Joint
                                            ///< space to Actuator space, actuator torque =
                                            ///< actuator transmission * joint torque
    double  position_sensor_transmission_;  ///< transmission ratio describing the
                                            ///< multiplicative gain in position
                                            ///< from PositionSensor space to Joint
                                            ///< space, joint position = position
                                            ///< sensor transmission * sensed
                                            ///< position
    double velocity_sensor_transmission_;   ///< transmission ratio describing the
                                            ///< multiplicative gain in velocity
                                            ///< from VelocitySensor space to Joint
                                            ///< space, joint velocity = velocity
                                            ///< sensor transmission * sensed
                                            ///< velocity

    double torque_;    ///< the stored torque of the Joint since the last call to
                       ///< set_torque() or add_torque()
    double position_;  ///< the stored position of the Joint since the last call
                       ///< to get_position()
    double velocity_;  ///< the stored velocity of the Joint since the last call
                       ///< to get_velocity()

    bool saturate_;  ///< command torques will be saturated at the torque limit
                     ///< if this is true

    double torque_limit_;  ///< the absolute limit on torque that should be
                           ///< allowed to the Joint
    std::array<double, 2>
        position_limits_;    ///< the [min, max] position limits of the Joint
    double velocity_limit_;  ///< the absolute limit on the Joint's velocity

    bool has_torque_limit_;  ///< whether or not the Joint should enforce torque
                             ///< limits
    bool has_position_limits_;  ///< whether or not the Joint should check
                                ///< position limits
    bool has_velocity_limit_;   ///< whether or not the Joint should check
                                ///< velocity limits
};

}  // namespace mel
