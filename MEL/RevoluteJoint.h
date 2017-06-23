#pragma once
#include "Joint.h"

class RevoluteJoint : public Joint {

public:

	// constructor
    RevoluteJoint(Encoder* encoder, Actuator* actuator)
        : Joint(encoder, actuator) {}
    
    double encoder_counts_to_joint_space(double counts);
    double joint_torque_to_actuator_torque(double joint_torque);

};

