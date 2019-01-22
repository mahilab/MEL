#include <MEL/Mechatronics/Actuator.hpp>

namespace mel {

Actuator::Actuator() : 
    Device("UNAMED_ACTUATOR"),
    torque_command_(0.0)
{
}

Actuator::Actuator(const std::string& name) :
    Device(name),
    torque_command_(0.0)
{ }

void Actuator::set_torque(double torque) {
    torque_command_ = torque;
}

double Actuator::get_torque_command() const {
    return torque_command_;
}

double Actuator::get_torque_limited() const {
    return torque_command_;
}

double Actuator::get_torque_sense() const {
    return 0.0;
}

} // namespace mel
