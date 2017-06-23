#include "RevoluteJoint.h"

double RevoluteJoint::encoder_counts_to_joint_space(double counts) {
    return 2.0 * PI * encoder_->transmission_ / (encoder_->counts_per_revolution_ * encoder_->quadrature_factor_) * encoder_->count_;
}

double RevoluteJoint::joint_torque_to_actuator_torque(double joint_torque) {
    return actuator_->transmission_ * joint_torque;
}

