#pragma once
#include "PositionSensor.h"
#include "Actuator.h"

namespace mel {

    class Joint {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        Joint();
        Joint(std::string name, PositionSensor* position_sensor, double position_sensor_transmission_, Actuator* actuator, double actuator_transmission);

        // PUBLIC FUNCTIONS

        virtual void enable();
        virtual void disable();

        double get_position(); // converts PositionSensor position to RobotJoint position
        double get_velocity(); // converts PositionSensor velocity to RobotJoint velocity
        double get_torque();   // returns last set torque

        void set_torque(double joint_torque);

        // PUBLIC VARIABLES

        std::string name_;

        PositionSensor* position_sensor_;
        Actuator* actuator_;
        
        const double position_sensor_transmission_;
        const double actuator_transmission_;

    protected:

        // STATE VARIABLES

        double position_;
        double velocity_;
        double torque_;


    };
}