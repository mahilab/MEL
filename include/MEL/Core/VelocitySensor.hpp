#pragma once

#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class VelocitySensor : public Device {

public:

    /// Default constructor
    VelocitySensor();

    /// Prefered constructor
    VelocitySensor(std::string name);

    /// This function should return the velocity of the VelocitySensor
    virtual double get_velocity() = 0;

protected:

    double velocity_; ///< stores the VelocitySensor velocity since the last call to get_velocity()

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
