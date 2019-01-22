#include <MEL/Mechatronics/PositionSensor.hpp>
#include <iostream>
#include <string>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

PositionSensor::PositionSensor() :
    position_(0.0)
{ }

PositionSensor::~PositionSensor() {

}

double PositionSensor::get_position() {
    return position_;
}

} // namespace mel
