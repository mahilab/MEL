#include <MEL/Core/PositionSensor.hpp>
#include <iostream>
#include <string>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

PositionSensor::PositionSensor(const std::string& name) :
    Device("PositionSensor::" + name),
    position_(0.0)
{ }

double PositionSensor::get_position() {
    return position_;
}

} // namespace mel
