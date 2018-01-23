#include <MEL/Core/Motor.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Clock.hpp>

namespace mel {

Motor::Motor(const std::string& name,
             double kt,
             Amplifier amplifier,
             Limiter current_limiter) :
    Actuator(name),
    kt_(kt),
    amplifier_(amplifier),
    current_limiter_(current_limiter)
{ }

bool Motor::enable() {
    if (amplifier_.enable())
        return Device::enable();
    else
        return false;
}

bool Motor::disable() {
    if (amplifier_.disable())
        return Device::disable();
    else
        return false;
}

void Motor::set_torque(double torque) {
    torque_command_ = torque;
    amplifier_.set_current(current_limiter_.limit(torque_command_ / kt_));
}

double Motor::get_torque_command() {
    return torque_command_;
}

double Motor::get_torque_limited() {
    return kt_ * amplifier_.get_current_limited();
}

double Motor::get_torque_sense() {
    return kt_ * amplifier_.get_current_sense();
}

} // namespace mel
