#include "../include/PrismaticJoint.h"

namespace mel {

    PrismaticJoint::PrismaticJoint(Encoder* encoder, Actuator* actuator) :
        Joint(encoder, actuator)
    {
    }

    double PrismaticJoint::encoder_to_joint_space(double counts) {
        return 2.0 * PI * encoder_->radius_ / (encoder_->counts_per_revolution_ * encoder_->quadrature_factor_) * encoder_->count_;
    }

    double PrismaticJoint::joint_torque_to_actuator_torque(double joint_torque) {
        return actuator_->radius_ * joint_torque;
    }

}