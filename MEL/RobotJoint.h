#pragma once
#include "PositionSensor.h"
#include "Actuator.h"

namespace mel {

    class RobotJoint {

    public:

        // constructor
        RobotJoint();
        RobotJoint(PositionSensor* position_sensor, double position_sensor_transmission_, Actuator* actuator, double actuator_transmission);

        // references
        PositionSensor* position_sensor_;
        Actuator* actuator_;
        
        const double position_sensor_transmission_;
        const double actuator_transmission_;

        // getters of state variables
        double get_position();
        double get_velocity();
        void set_torque(double joint_torque);
        

    private:

        // state variables
        double position_;
        double velocity_;
        double torque_;


    };
}