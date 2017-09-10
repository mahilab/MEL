#pragma once
#include "Device.h"
#include "Joint.h"
#include "Actuator.h"
#include "PositionSensor.h"
#include "mel_util.h"

namespace mel {

    namespace core {

        class Robot : public Device {

        public:

            //---------------------------------------------------------------------
            // CONSTRUCTOR(S) / DESTRUCTOR(S)
            //---------------------------------------------------------------------

            /// Default constructor.
            Robot() : Device("robot") {}
            /// Prefered constructor.
            Robot(std::string name) : Device(name) {}
            /// Destructor
            virtual ~Robot() { }

            //---------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //---------------------------------------------------------------------

            /// Virtual function to enable all robot members.
            virtual void enable() override;
            /// Virtual function to disable all robot members.
            virtual void disable() override;

            /// Get the most recently read robot joint positions.
            double_vec get_joint_positions();
            /// Get the most recently read robot joint velocities.
            double_vec get_joint_velocities();
            /// Set the desired actuator torques.
            void set_joint_torques(double_vec new_torques);

            /// Checks position limits of all joints and returns true if any exceeded, false otherwise
            bool check_all_joint_position_limits();
            /// Checks velocity limits of all joints and returns true if any exceeded, false otherwise
            bool check_all_joint_velocity_limits();
            /// Checks torque limits of all joints and returns true if any exceeded, false otherwise
            bool check_all_joint_torque_limits();
            /// Checks position, velocity, and torque limits of all joints and returns true if any exceeded, false otherwise
            bool check_all_joint_limits();

            //---------------------------------------------------------------------
            // PUBLIC VARIABLES
            //---------------------------------------------------------------------

            std::vector<Joint*> joints_; ///< Vector of RobotJoint pointers.        
            std::vector<PositionSensor*> position_sensors_; ///< Vector of PositionSensor pointers.        
            std::vector<Actuator*> actuators_; ///< Vector of Actuator pointers.     

        protected:

            //---------------------------------------------------------------------
            // PROTECTED VARIABLES
            //---------------------------------------------------------------------

            double_vec joint_positions_; ///< Stores the robot joint positions since the last call of get_robot_joint_positions().      
            double_vec joint_velocities_; ///< Stores the robot joint velocities since the last call of get_robot_joint_velocities().
            double_vec joint_torques_; ///< Does this need to exist?

        };

    }

}