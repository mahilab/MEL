#include <MEL/Core/Joint.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Console.hpp>
#include <iostream>

namespace mel {

 Joint::Joint(std::string name,
        PositionSensor& position_sensor, double position_sensor_transmission,
        VelocitySensor& velocity_sensor, double velocity_sensor_transmission,
        Actuator& actuator, double actuator_transmission,
        std::array<double, 2> position_limits,
        double velocity_limit,
        double torque_limit,
        bool saturate) :
    Device(name),
    position_sensor_(position_sensor),
    position_sensor_transmission_(position_sensor_transmission),
    velocity_sensor_(velocity_sensor),
    velocity_sensor_transmission_(velocity_sensor_transmission),
    actuator_(actuator),
    actuator_transmission_(actuator_transmission),
    position_(0.0),
    position_limits_(position_limits),
    has_position_limits_(true),
    velocity_(0.0),
    velocity_limit_(velocity_limit),
    has_velocity_limit_(true),
    torque_(0.0),
    torque_limit_(torque_limit),
    has_torque_limit_(true),
    saturate_(saturate)
{ }

bool Joint::enable() {
    if (position_sensor_.enable() && velocity_sensor_.enable() && actuator_.enable()) {
        return Device::enable();
    }
    return false;
}

bool Joint::disable() {
    if (position_sensor_.disable() && velocity_sensor_.disable() && actuator_.disable()) {
        return Device::disable();
    }
    return false;
}

double Joint::get_position() {
    position_ = position_sensor_transmission_ * position_sensor_.get_position();
    return position_;
}

double Joint::get_velocity() {
    velocity_ = velocity_sensor_transmission_ * velocity_sensor_.get_velocity();
    return velocity_;
}

double Joint::get_torque() {
    return torque_;
}

void Joint::set_torque(double new_torque) {
    torque_ = new_torque;
    if (check_torque_limit() && saturate_) {
        print("WARNING: Joint " + namify(name_) + " command torque saturated to " + std::to_string(torque_limit_) + ".");
        torque_ = saturate(torque_, torque_limit_);
    }
    actuator_.set_torque(actuator_transmission_ * torque_);
}

void Joint::add_torque(double additional_torque) {
    torque_ += additional_torque;
    set_torque(torque_);
}

bool Joint::check_torque_limit() {
    bool exceeded = false;
    if (has_torque_limit_ && abs(torque_) > torque_limit_) {
        print("WARNING: Joint " + namify(name_) + " command torque exceeded the torque limit " + std::to_string(torque_limit_) + " with a value of " + std::to_string(torque_) + ".");
        exceeded = true;
    }
    return exceeded;
}

bool Joint::check_position_limits() {
    get_position();
    bool exceeded = false;
    if (has_position_limits_ && position_ < position_limits_[0]) {
        print("WARNING: Joint " + namify(name_) + " position exceeded the min position limit " + std::to_string(position_limits_[0]) + " with a value of " + std::to_string(position_) + ".");
        exceeded = true;
    }
    if (has_position_limits_ && position_ > position_limits_[1]) {
        print("WARNING: Joint " + namify(name_) + " position exceeded the max position limit " + std::to_string(position_limits_[1]) + " with a value of " + std::to_string(position_) + ".");
        exceeded = true;
    }
    return exceeded;
}

bool Joint::check_velocity_limit() {
    get_velocity();
    bool exceeded = false;
    if (has_velocity_limit_ && abs(velocity_) > velocity_limit_) {
        print("WARNING: Joint " + namify(name_) + " velocity exceeded the velocity limit " + std::to_string(velocity_limit_) + " with a value of " + std::to_string(velocity_) + ".");
        exceeded = true;
    }
    return exceeded;
}

bool Joint::check_all_limits() {
    bool exceeded = false;
    if (check_position_limits())
        exceeded = true;
    if (check_velocity_limit())
        exceeded = true;
    if (check_torque_limit())
        exceeded = true;
    return exceeded;
}

} // namespace mel
