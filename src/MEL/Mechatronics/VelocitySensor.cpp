#include <MEL/Mechatronics/VelocitySensor.hpp>
#include <iostream>
#include <string>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

VelocitySensor::VelocitySensor() :
    velocity_(0.0)
{ }

VelocitySensor::~VelocitySensor() {

}

double VelocitySensor::get_velocity() {
    return velocity_;
}

} // namespace mel
