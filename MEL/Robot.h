#pragma once
#include "RobotJoint.h"
#include "RevoluteRobotJoint.h"
#include "PrismaticRobotJoint.h"
#include "Actuator.h"
#include "Motor.h"
#include "PositionSensor.h"
#include "Encoder.h"
#include "util.h"
#include <vector>


namespace mel {

    /// \brief The base class for all robots.
    
    /// Detailed description goes here.
    ///

    class Robot {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        /// Default constructor.
        Robot() {}


        // PUBLIC FUNCTIONS

        /// Virtual function to enable all robot members.
        virtual void enable() {}

        /// Virtual function to disable all robot members.
        virtual void disable() {}

        /// Get the most recently read robot joint positions.
        double_vec get_robot_joint_positions();

        /// Get the most recently read robot joint velocities.
        double_vec get_robot_joint_velocities();

        /// Set the desired actuator torques.
        void set_robot_joint_torques(double_vec new_torques);

        // PUBLIC VARIABLES

        /// Vector of RobotJoint pointers.
        std::vector<RobotJoint*> robot_joints_;

        /// Vector of PositionSensor pointers.
        std::vector<PositionSensor*> position_sensors_;

        /// Vector of Actuator pointers.
        std::vector<Actuator*> actuators_;

        

    protected:

        // STATE VARIABLES

        /// Stores the robot joint positions since the last call of get_robot_joint_positions(). 
        double_vec robot_joint_positions_;
        
        /// Stores the robot joint velocities since the last call of get_robot_joint_velocities().
        double_vec robot_joint_velocities_;
        
        /// Does this need to exist?
        double_vec robot_joint_torques_;

    };
}