#pragma once
#include "Joint.h"

namespace mel {

    class PrismaticJoint : public Joint {

    public:

        // constructors
        PrismaticJoint() {}
        PrismaticJoint(Encoder* encoder, Actuator* actuator);
        
        double encoder_to_joint_space(double counts);
        double joint_torque_to_actuator_torque(double joint_torque);
        
    };

}