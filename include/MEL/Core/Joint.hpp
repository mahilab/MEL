#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Core/PositionSensor.hpp>
#include <MEL/Core/VelocitySensor.hpp>
#include <MEL/Core/Actuator.hpp>
#include <array>
#include <memory>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Joint : public Device {

public:

    /// Default constructor
    Joint(const std::string& name, 
        Actuator& actuator,
        double actuator_transmission,
        PositionSensor& position_sensor, 
        double position_sensor_transmission,
        VelocitySensor& velocity_sensor, 
        double velocity_sensor_transmission,
        std::array<double, 2> position_limits, 
        double velocity_limit, 
        double torque_limit, 
        bool saturate = true);

    /// Enables the joint's position sensor, velocity sensor, and actuator
    bool enable() override;

    /// Disables the joint's position sensor, velocity sensor, and actuator
    bool disable() override;

    /// Converts PositionSensor position to Joint position
    double get_position();

    /// Converts PositionSensor velocity to Joint velocity
    double get_velocity();

    /// Returns the currently set joint torque
    double get_torque();

    /// Sets the joint torque to #new_torque
    void set_torque(double new_torque);

    /// Adds #additional_torque to the currently set torque
    void add_torque(double additional_torque);

    /// Gets current position, checks it against limits, and returns true if min or max exceeded, false otherwise
    bool position_limit_exceeded();

    /// Gets current velocity, checks it against limit, and returns true if exceeded, false otherwise
    bool velocity_limit_exceeded();

    ///  Gets last commanded torque, checks it against torque limit, and returns true if exceeded, false otherise
    bool torque_limit_exceeded();

    /// Gets current position, velocity, and torque, checks them against limits, and returns true if either exceeded, false otherwise
    bool any_limit_exceeded();

protected:

    Actuator&       actuator_;
    PositionSensor& position_sensor_;
    VelocitySensor& velocity_sensor_;

    const double actuator_transmission_;
    const double position_sensor_transmission_;
    const double velocity_sensor_transmission_;

    std::array<double, 2> position_limits_; ///< the [min, max] position limits of the Joint
    double velocity_limit_; ///< the absolute limit on the Joint's velocity
    bool has_torque_limit_; ///< whether or not the Joint should enforce torque limits

    bool saturate_; ///< command torques will be saturated at the torque limit if this is true

    double position_; ///< the stored position of the Joint since the last call to get_position()
    bool has_position_limits_; ///< whether or not the Joint should check position limits

    double velocity_; ///< the stored velocity of the Joint since the last call to get_velocity()
    bool has_velocity_limit_; ///< whether or not the Joint should check velocity limits

    double torque_; ///< the stored torque of the Joint since the last call to set_torque() or add_torque()
    double torque_limit_; ///< the absolute limit on torque that should be allowed to the Joint

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
