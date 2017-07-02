#include "PositionSensor.h"

namespace mel {

    PositionSensor::PositionSensor() :
        position_(0.0),
        velocity_(0.0)
    {

    }

    double PositionSensor::get_velocity() {
        return velocity_;
    }
}