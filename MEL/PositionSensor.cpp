#include "PositionSensor.h"

namespace mel {

    PositionSensor::PositionSensor() :
        name_("no_name"),
        velocity_enabled_(false),
        position_(0.0),
        velocity_(0.0)
    { }

    PositionSensor::PositionSensor(std::string name, bool velocity_enabled) :
        name_(name),
        velocity_enabled_(velocity_enabled),
        position_(0.0),
        velocity_(0.0)
    { }

    double PositionSensor::get_velocity() {
        std::cout << "WARNING: PositionSensor <" << name_ << "> was not constructed to enable velocity measurement. Returning 0." << std::endl;
        return 0.0;
    }

}