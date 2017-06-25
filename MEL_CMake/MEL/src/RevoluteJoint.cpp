#include "../include/RevoluteJoint.h"

namespace mel {

    RevoluteJoint::RevoluteJoint(double radius, Encoder* encoder, Actuator* actuator) :
        radius_(radius),
        Joint(encoder, actuator)
    {
    }

    double RevoluteJoint::encoder_to_joint_space(double counts) {
        return 2.0 * PI * encoder_->radius_ / (radius_ * encoder_->counts_per_revolution_ * encoder_->quadrature_factor_) * encoder_->count_;
    }

    double RevoluteJoint::joint_torque_to_actuator_torque(double joint_torque) {
        return (actuator_->radius_ / radius_) * joint_torque;
    }
}