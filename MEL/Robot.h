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

    class Robot {

    public:

        // constructor
        Robot() {}

        // components
        std::vector<RobotJoint*> robot_joints_;
        std::vector<PositionSensor*> position_sensors_;
        std::vector<Actuator*> actuators_;

        // getters and setters of state variables
        double_vec get_robot_joint_positions();
        double_vec get_robot_joint_velocities();
        void set_robot_joint_torques(double_vec new_torques);

    protected:

        // state variables
        double_vec robot_joint_positions_;
        double_vec robot_joint_velocities_;
        double_vec robot_joint_torques_;

    };
}