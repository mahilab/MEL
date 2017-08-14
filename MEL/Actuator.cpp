#include "Actuator.h"

namespace mel {

    Actuator::Actuator() :
        Device("actuator"),
        torque_(0.0),
        enable_mode_(EnableMode::None)
    { }

    Actuator::Actuator(std::string name, EnableMode enable_mode) :
        Device(name),
        torque_(0.0),
        enable_mode_(enable_mode)
    { }

}