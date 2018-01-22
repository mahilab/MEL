#pragma once

#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Actuator : public Device {

public:

    /// The digital TT: logic level required to enable the Actuator
    enum EnableMode {
        High,  ///< High TTL level (typically 5V)
        Low,   ///< Low TTL level (typically 0V)
    };

public:

    /// Default constructor
    Actuator();

    /// Constructor for actuator without torque limits
    Actuator(std::string name, EnableMode enable_mode);

    /// This function should set the desired torque to be generated at the Actuator
    virtual void set_torque(double torque) = 0;

    /// This function should return the attempted command current
    virtual double get_torque_command();

    /// This function should return the limited command current if limitations are imposed
    virtual double get_torque_limited();

    /// This function should return the torque sense of the actuator if it is available
    virtual double get_torque_sense();

protected:

    EnableMode enable_mode_; ///< The enable mode of the actuator (None, Low, or High)
    double torque_command_;  ///< Stores the Actuator torque since the last call to set_torque()
    double torque_sense_;    ///< Stores the Actuator torque sense since the last call to get_torque_sense();

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
