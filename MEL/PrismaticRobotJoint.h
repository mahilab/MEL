#pragma once
#include "RobotJoint.h"

namespace mel {

    class PrismaticRobotJoint : public RobotJoint {

    public:

        // constructors
        PrismaticRobotJoint() {}
        PrismaticRobotJoint(PositionSensor* position_sensor, double position_sensor_transmission, Actuator* actuator, double actuator_transmission);
        
    private:

        
    };

}