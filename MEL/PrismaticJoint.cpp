#include "PrismaticJoint.h"

double PrismaticJoint::get_position() {
    return encoder_counts_to_joint_positions(encoder_->get_counts());
}

double PrismaticJoint::get_velocity() {
    return encoder_counts_to_joint_positions(encoder_->get_velocity());
}

double RevoluteJoint::encoder_counts_to_joint_positions(double counts) {
    return 2.0 * PI*transmission_*counts;
}

double RevoluteJoint::encoder_counts_per_sec_to_joint_velocity(double counts_per_sec) {
    return 2.0 * PI*transmission_*counts_per_sec;
}

void RevoluteJoint::set_actuator_torque(double torque) {
    actuator_->set_torque(torque);