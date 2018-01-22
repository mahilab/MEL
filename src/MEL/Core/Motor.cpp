#include <MEL/Core/Motor.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Clock.hpp>

namespace mel {

Motor::Motor() :
    Actuator()
{ }

/// Constructor for digitally enabled, analog voltage controlled DC motor
Motor::Motor(std::string name,
             double kt,
             double amp_gain,
             EnableMode enable_mode,
             Output<logic>::Channel do_channel,
             Output<voltage>::Channel ao_channel,
             Input<voltage>::Channel ai_channel,
             Limiter current_limiter) :
    Actuator(name, enable_mode),
    kt_(kt),
    kt_inv_(1.0 / kt),
    amp_gain_(amp_gain),
    do_channel_(do_channel),
    ao_channel_(ao_channel),
    ai_channel_(ai_channel),
    current_limiter_(current_limiter),
    current_command_(0.0),
    current_sense_(0.0)
{
}

Motor::Motor(std::string name,
             double kt,
             double amp_gain,
             EnableMode enable_mode,
             Output<logic>::Channel do_channel,
             Output<voltage>::Channel ao_channel,
             Limiter current_limiter) :
    Actuator(name, enable_mode),
    kt_(kt),
    kt_inv_(1.0 / kt),
    amp_gain_(amp_gain),
    do_channel_(do_channel),
    ao_channel_(ao_channel),
    ai_channel_(Input<voltage>::Channel()),
    current_limiter_(current_limiter),
    current_command_(0.0),
    current_sense_(0.0)
{

}

bool Motor::enable() {
    current_limiter_.reset();
    ao_channel_.set_value(0.0);
    if (enable_mode_ == EnableMode::High)
        do_channel_.set_value(HIGH);
    else if (enable_mode_ == EnableMode::Low)
        do_channel_.set_value(LOW);
    if (do_channel_.update())
        return Device::enable();
    else
        return false;
}

bool Motor::disable() {
    ao_channel_.set_value(0.0);
    if (enable_mode_ == EnableMode::High)
        do_channel_.set_value(LOW);
    else if (enable_mode_ == EnableMode::Low)
        do_channel_.set_value(HIGH);
    if (do_channel_.update())
        return Device::disable();
    else
        return false;
}

void Motor::set_torque(double torque) {
    torque_command_ = torque;
    set_current(kt_inv_ * torque_command_);
}

double Motor::get_torque_command() {
    return torque_command_;
}

double Motor::get_torque_limited() {
    return kt_ * current_limiter_.get_limited_value();
}

double Motor::get_torque_sense() {
    return kt_ * get_current_sense();
}

void Motor::set_current(double current) {
    current_command_ = current;
    ao_channel_.set_value(current_limiter_.limit(current_command_) / amp_gain_);
}

double Motor::get_current_command() {
    return current_command_;
}

double Motor::get_current_limited() {
    return current_limiter_.get_limited_value();
}

double Motor::get_current_sense() {
    if (ai_channel_.is_valid()) {
        current_sense_ = ai_channel_.get_value();
        return current_sense_;
    }
    std::cout << "WARNING: Motor <" << name_ << "> was not constructed to enable current sense. Returning 0." << std::endl;
    return 0.0;
}

} // namespace mel
