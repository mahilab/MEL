#include <MEL/Core/PositionSensor.hpp>
#include <iostream>
#include <string>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

PositionSensor::PositionSensor() :
    Device("invalid_position_sensor"),
    position_(0.0)
{ }

PositionSensor::PositionSensor(std::string name) :
    Device(name),
    position_(0.0)
{ }

double PositionSensor::get_position() {
    return position_;
}

} // namespace mel
