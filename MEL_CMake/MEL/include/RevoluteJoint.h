#pragma once
#include "Joint.h"

namespace mel {

    class RevoluteJoint : public Joint {

    public:

        // constructors
        RevoluteJoint() {}
        RevoluteJoint(double radius, Encoder* encoder, Actuator* actuator);

        // parameters
        const double radius_{};

        double encoder_to_joint_space(double counts);
        double joint_torque_to_actuator_torque(double joint_torque);

    };
}