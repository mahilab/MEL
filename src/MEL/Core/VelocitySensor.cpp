#include <MEL/Core/VelocitySensor.hpp>
#include <iostream>
#include <string>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

VelocitySensor::VelocitySensor(const std::string& name) :
    Device(name),
    velocity_(0.0)
{ }

double VelocitySensor::get_velocity() {
    return velocity_;
}

} // namespace mel
