#pragma once
#include "Device.h"
#include "PositionSensor.h"
#include "Actuator.h"

namespace mel {

    class Joint : public Device {

    public:

        //---------------------------------------------------------------------
        // CONSTRUCTOR(S) / DESTRUCTOR(S)
        //---------------------------------------------------------------------

        Joint();
        Joint(std::string name, PositionSensor* position_sensor, double position_sensor_transmission_, Actuator* actuator, double actuator_transmission);

        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        virtual void enable() override;
        virtual void disable() override;

        /// Converts PositionSensor position to RobotJoint position
        double get_position(); 
        /// Converts PositionSensor velocity to RobotJoint velocity
        double get_velocity(); 
        /// Returns last set joint torque
        double get_torque();   
        /// Sets the joint torque
        void set_torque(double joint_torque);

        //---------------------------------------------------------------------
        // PUBLIC VARIABLES
        //---------------------------------------------------------------------

        PositionSensor* position_sensor_;
        Actuator* actuator_;
        
        const double position_sensor_transmission_;
        const double actuator_transmission_;

    protected:

        //---------------------------------------------------------------------
        // STATE VARIABLES
        //---------------------------------------------------------------------

        double position_;
        double velocity_;
        double torque_;


    };
}