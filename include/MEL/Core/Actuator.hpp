#pragma once

#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Actuator : public Device {

public:

    /// Constructor for actuator without torque limits
    Actuator(const std::string& name);

    /// This function should set the desired torque to be generated at the Actuator
    virtual void set_torque(double torque) = 0;

    /// This function should return the attempted command current
    virtual double get_torque_command();

    /// This function should return the limited command current if limitations are imposed
    virtual double get_torque_limited();

    /// This function should return the torque sense of the actuator if it is available
    virtual double get_torque_sense();

protected:

    double torque_command_;  ///< Stores the Actuator torque since the last call to set_torque()

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
