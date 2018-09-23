#include <MEL/Mechatronics/Joint.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <iostream>

namespace mel {

 Joint::Joint(const std::string& name,
        Actuator* actuator,
        double actuator_transmission,
        PositionSensor* position_sensor,
        double position_sensor_transmission,
        VelocitySensor* velocity_sensor,
        double velocity_sensor_transmission,
        std::array<double, 2> position_limits,
        double velocity_limit,
        double torque_limit,
        bool saturate) :
    Device(name),
    actuator_(actuator),
    position_sensor_(position_sensor),
    velocity_sensor_(velocity_sensor),
    actuator_transmission_(actuator_transmission),
    position_sensor_transmission_(position_sensor_transmission),
    velocity_sensor_transmission_(velocity_sensor_transmission),
    torque_(0.0),
    position_(0.0),
    velocity_(0.0),
    saturate_(saturate),
    torque_limit_(torque_limit),
    position_limits_(position_limits),
    velocity_limit_(velocity_limit),
    has_torque_limit_(true),
    has_position_limits_(true),
    has_velocity_limit_(true)
{ }

 Joint::Joint(const std::string& name,
     Actuator* actuator,
     PositionSensor* position_sensor,
     VelocitySensor* velocity_sensor,
     double transmission,
     std::array<double, 2> position_limits,
     double velocity_limit,
     double torque_limit,
     bool saturate) :
     Device("Joint::" + name),
     actuator_(actuator),
     position_sensor_(position_sensor),
     velocity_sensor_(velocity_sensor),
     actuator_transmission_(transmission),
     position_sensor_transmission_(transmission),
     velocity_sensor_transmission_(transmission),
     torque_(0.0),
     position_(0.0),
     velocity_(0.0),
     saturate_(saturate),
     torque_limit_(torque_limit),
     position_limits_(position_limits),
     velocity_limit_(velocity_limit),
     has_torque_limit_(true),
     has_position_limits_(true),
     has_velocity_limit_(true)
 { }

bool Joint::on_enable() {
    if (actuator_->enable())
        return true;
    return false;
}

bool Joint::on_disable() {
    if (actuator_->disable())
        return true;
    return false;
}

double Joint::get_position() {
    position_ = position_sensor_transmission_ * position_sensor_->get_position();
    return position_;
}

double Joint::get_velocity() {
    velocity_ = velocity_sensor_transmission_ * velocity_sensor_->get_velocity();
    return velocity_;
}

double Joint::get_torque_command() {
    return torque_;
}

double Joint::get_torque_sense() {
    return actuator_->get_torque_sense() / actuator_transmission_;
}

void Joint::set_torque(double new_torque) {
    torque_ = new_torque;
    if (torque_limit_exceeded() && saturate_) {
        LOG(Warning) << "Joint " << get_name() << " command torque saturated to " << torque_limit_;
        torque_ = saturate(torque_, torque_limit_);
    }
    actuator_->set_torque(actuator_transmission_ * torque_);
}

void Joint::add_torque(double additional_torque) {
    torque_ += additional_torque;
    set_torque(torque_);
}

bool Joint::torque_limit_exceeded() {
    bool exceeded = false;
    if (has_torque_limit_ && abs(torque_) > torque_limit_) {
        LOG(Warning) << "Joint " << get_name() << " command torque exceeded the torque limit " << torque_limit_ << " with a value of " << torque_;
        exceeded = true;
    }
    return exceeded;
}

bool Joint::position_limit_exceeded() {
    get_position();
    bool exceeded = false;
    if (has_position_limits_ && position_ < position_limits_[0]) {
        LOG(Warning) << "Joint " << get_name() << " position exceeded the min position limit " << position_limits_[0] << " with a value of " << position_;
        exceeded = true;
    }
    if (has_position_limits_ && position_ > position_limits_[1]) {
        LOG(Warning) << "Joint " << get_name() << " position exceeded the max position limit " << position_limits_[1] << " with a value of " << position_;
        exceeded = true;
    }
    return exceeded;
}

bool Joint::velocity_limit_exceeded() {
    get_velocity();
    bool exceeded = false;
    if (has_velocity_limit_ && abs(velocity_) > velocity_limit_) {
        LOG(Warning) << "Joint " << get_name() << " velocity exceeded the velocity limit " << velocity_limit_ << " with a value of " << velocity_;
        exceeded = true;
    }
    return exceeded;
}

bool Joint::any_limit_exceeded() {
    bool exceeded = false;
    if (position_limit_exceeded())
        exceeded = true;
    if (velocity_limit_exceeded())
        exceeded = true;
    if (torque_limit_exceeded())
        exceeded = true;
    return exceeded;
}

} // namespace mel
