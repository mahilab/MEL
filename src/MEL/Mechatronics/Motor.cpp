#include <MEL/Mechatronics/Motor.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Core/Clock.hpp>

namespace mel {

Motor::Motor() :
    Actuator("UNAMED_MOTOR"),
    kt(0.0),
    amplifier_(),
    current_limiter_()
{

}

Motor::Motor(const std::string& name,
             double _kt,
             Amplifier amplifier,
             Limiter current_limiter) :
    Actuator(name),
    kt(_kt),
    amplifier_(amplifier),
    current_limiter_(current_limiter)
{ }

bool Motor::on_enable() {
    if (amplifier_.enable())
        return true;
    else
        return false;
}

bool Motor::on_disable() {
    if (amplifier_.disable())
        return true;
    else
        return false;
}

void Motor::set_torque(double torque) {
    torque_command_ = torque;
    amplifier_.set_current(current_limiter_.limit(torque_command_ / kt));
}

double Motor::get_torque_command() const {
    return torque_command_;
}

double Motor::get_torque_limited() const {
    return kt * amplifier_.get_current_limited();
}

double Motor::get_torque_sense() const {
    return kt * amplifier_.get_current_sense();
}

Amplifier& Motor::get_amplifier() {
    return amplifier_;
}

} // namespace mel
