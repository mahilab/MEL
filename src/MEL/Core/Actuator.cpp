#include <MEL/Core/Actuator.hpp>

namespace mel {

Actuator::Actuator(const std::string& name) :
    Device(name),
    torque_command_(0.0)
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
    return 0.0;
}

} // namespace mel
