#include "Actuator.h"

namespace mel {

    Actuator::Actuator() :
        Device("actuator"),
        torque_(0.0),
        enable_mode_(EnableMode::None),
        has_torque_limit_(false)
    { }

    Actuator::Actuator(std::string name, EnableMode enable_mode) :
        Device(name),
        torque_(0.0),
        enable_mode_(enable_mode),
        has_torque_limit_(false)        
    { }

    Actuator::Actuator(std::string name, EnableMode enable_mode, double torque_limit) :
        Device(name),
        torque_(0.0),
        enable_mode_(enable_mode),
        torque_limit_(torque_limit),
        has_torque_limit_(true)
    { }

}