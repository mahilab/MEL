#include "../lib/Joint.h"

namespace mel {

    Joint::Joint(Encoder* encoder, Actuator* actuator) : 
        encoder_(encoder), actuator_(actuator)
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

    void Joint::set_actuator_torque(double new_joint_torque) {
        actuator_->set_torque(joint_torque_to_actuator_torque(new_joint_torque));
    }
}