#pragma once
#include "RobotJoint.h"

namespace mel {

    class RevoluteRobotJoint : public RobotJoint {

    public:

        // constructors
        RevoluteRobotJoint() {}
        RevoluteRobotJoint(PositionSensor* position_sensor, double position_sensor_transmission, Actuator* actuator, double actuator_transmission);

    private:



    };
}