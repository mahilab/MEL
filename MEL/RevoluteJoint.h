#pragma once
#include "Joint.h"

class RevoluteJoint : public Joint {

public:

	// constructors
    RevoluteJoint() {}
    RevoluteJoint(Encoder* encoder, Actuator* actuator);
    
    double encoder_to_joint_space(double counts);
    double joint_torque_to_actuator_torque(double joint_torque);

};

