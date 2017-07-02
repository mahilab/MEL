#include "PositionSensor.h"

namespace mel {

    PositionSensor::PositionSensor() :
        position_(0.0),
        velocity_(0.0),
        velocity_enabled_(false)
    { }

    PositionSensor::PositionSensor(bool velocity_enabled) :
        position_(0.0),
        velocity_(0.0),
        velocity_enabled_(velocity_enabled)
    { }

}