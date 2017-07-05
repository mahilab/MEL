#include "Joint.h"
#include <iostream>

namespace mel {

    Joint::Joint() :
        name_("no name"),
        position_sensor_(nullptr),
        actuator_(nullptr),
        position_sensor_transmission_(0.0),
        actuator_transmission_(0.0),
        position_(0.0),
        velocity_(0.0),
        torque_(0.0)
    { }

    Joint::Joint(std::string name, PositionSensor* position_sensor, double position_sensor_transmission, Actuator* actuator, double actuator_transmission) :
        name_(name),
        position_sensor_(position_sensor),
        actuator_(actuator),
        position_sensor_transmission_(position_sensor_transmission),
        actuator_transmission_(actuator_transmission),
        position_(0.0),
        velocity_(0.0),
        torque_(0.0)
    { }

    double Joint::get_position() {
        position_ = position_sensor_transmission_ * position_sensor_->get_position();
        return position_;
    }

    double Joint::get_velocity() {
        velocity_ = position_sensor_transmission_ * position_sensor_->get_velocity();
        return velocity_;
    }

    void Joint::set_torque(double joint_torque) {
        torque_ = joint_torque;
        actuator_->set_torque(actuator_transmission_ * torque_);
    }

    void Joint::enable() {
        position_sensor_->enable();
        actuator_->enable();
    }

    void Joint::disable() {
        position_sensor_->disable();
        actuator_->disable();
    }
}