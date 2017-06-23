#include "Joint.h"

double Joint::get_position() {
    position_ = encoder_counts_to_joint_positions(encoder_->get_counts);
}

double Joint::get_velocity() {
    velocity_ = encoder_counts_to_joint_positions(encoder_->get_velocity);
}

void Joint::set_actuator_torque(double joint_torque) {
    actuator_->set_torque(joint_torque_to_actuator_torque(joint_torque));
}