#include <MEL/Core/VelocitySensor.hpp>
#include <iostream>
#include <string>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

VelocitySensor::VelocitySensor() :
    Device("invalid_velocity_sensor"),
    velocity_(0.0)
{ }

VelocitySensor::VelocitySensor(std::string name) :
    Device(name),
    velocity_(0.0)
{ }

double VelocitySensor::get_velocity() {
    return velocity_;
}

} // namespace mel
