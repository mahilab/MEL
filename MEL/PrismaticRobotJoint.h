#pragma once
#include "RobotJoint.h"

namespace mel {

    class PrismaticRobotJoint : public RobotJoint {

    public:

        // constructors
        PrismaticRobotJoint() {}
        PrismaticRobotJoint(PositionSensor* position_sensor, Actuator* actuator);
        
    private:

        double encoder_to_joint_space(double counts);
        double joint_torque_to_actuator_torque(double joint_torque);
        
    };

}