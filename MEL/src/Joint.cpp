#include "Joint.h"

namespace mel {

    Joint::Joint() : 
        encoder_(nullptr),
        actuator_(nullptr),
        position_(0.0),
        velocity_(0.0),
        torque_(0.0)
    {
        
    }

    Joint::Joint(Encoder* encoder, Actuator* actuator) : 
        encoder_(encoder),
        actuator_(actuator),
        position_(0.0),
        velocity_(0.0),
        torque_(0.0)
    {

    }


    double Joint::get_position() {
        position_ = encoder_to_joint_space(encoder_->get_count());
        return position_;
    }

    double Joint::get_velocity() {
        velocity_ = encoder_to_joint_space(encoder_->get_count_rate());
        return velocity_;
    }

    void Joint::set_torque(double joint_torque) {
        torque_ = joint_torque;
        actuator_->set_torque(joint_torque_to_actuator_torque(joint_torque));
    }

    void Joint::update_position() {
        position_ = encoder_to_joint_space(encoder_->get_count());
    }
}