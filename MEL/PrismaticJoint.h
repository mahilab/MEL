#pragma once
#include "Joint.h"

class PrismaticJoint : public Joint {

public:

    // constructor
    PrismaticJoint(double transmission, Encoder* encoder, Actuator* actuator)
        : Joint(transmission, encoder, actuator) {}

    PrismaticJoint(double transmission, Encoder* encoder);

    // inherited virtual functions from Joint
    double get_position();
    double get_velocity();
    void set_actuator_torque(double torque);

    double encoder_counts_to_joint_positions(double counts);
    double encoder_counts_per_sec_to_joint_velocity(double counts_per_sec);

};