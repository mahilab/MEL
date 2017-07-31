#include "Actuator.h"

namespace mel {

    Actuator::Actuator() :
        name_("no_name"),
        torque_(0.0),
        enable_mode_(EnableMode::None)
    { }

    Actuator::Actuator(std::string name, EnableMode enable_mode) :
        name_(name),
        torque_(0.0),
        enable_mode_(enable_mode)
    { }

    void Actuator::enable() {
        std::cout << "WARNING: Actuator <" << name_ << "> was not constructed to use the function enable()." << std::endl;
    }

    void Actuator::disable() {
        std::cout << "WARNING: Actuator <" << name_ << "> was not constructed to use the function disable()." << std::endl;
    }
}