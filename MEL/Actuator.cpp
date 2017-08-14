#include "Actuator.h"

namespace mel {

    Actuator::Actuator() :
        Device("actuator"),
        torque_(0.0),
        enable_mode_(EnableMode::None),
        has_torque_limit_(false),
        saturate_(false)
    { }

    Actuator::Actuator(std::string name, EnableMode enable_mode) :
        Device(name),
        torque_(0.0),
        enable_mode_(enable_mode),
        has_torque_limit_(false)   ,
        saturate_(false)
    { }

    Actuator::Actuator(std::string name, EnableMode enable_mode, double torque_limit, bool saturate) :
        Device(name),
        torque_(0.0),
        enable_mode_(enable_mode),
        torque_limit_(torque_limit),
        has_torque_limit_(true),
        saturate_(saturate)
    { }

    bool Actuator::check_torque_limit() {
        bool exceeded = false;
        if (has_torque_limit_ && abs(torque_) > torque_limit_) {
            print("WARNING: Actuator " + namify(name_) + " command torque exceeded the torque limit " + std::to_string(torque_limit_) + " with a value of " + std::to_string(torque_) + ".");
            exceeded = true;
        }
        return exceeded;
    }

}