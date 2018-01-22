#include <MEL/Core/Actuator.hpp>

namespace mel {

Actuator::Actuator() :
    Device("invalid_actuator"),
    enable_mode_(High),
    torque_command_(0.0),
    torque_sense_(0.0)
{ }

Actuator::Actuator(std::string name, EnableMode enable_mode) :
    Device(name),
    enable_mode_(enable_mode),
    torque_command_(0.0),
    torque_sense_(0.0)
{ }

void Actuator::set_torque(double torque) {
    torque_command_ = torque;
}

double Actuator::get_torque_command() {
    return torque_command_;
}

double Actuator::get_torque_limited() {
    return torque_command_;
}

double Actuator::get_torque_sense() {
    return torque_sense_;
}

} // namespace mel
