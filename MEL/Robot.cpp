#include "Robot.h"

namespace mel {

    double_vec Robot::get_robot_joint_positions() {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            robot_joint_positions_[it - robot_joints_.begin()] = (*it)->get_position();
        }
        return robot_joint_positions_;
    }

    double_vec Robot::get_robot_joint_velocities() {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            robot_joint_velocities_[it - robot_joints_.begin()] = (*it)->get_velocity();
        }
        return robot_joint_velocities_;
    }

    void Robot::set_robot_joint_torques(double_vec new_torques) {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            (*it)->set_torque(new_torques[it-robot_joints_.begin()]);
        }
    }

    void Robot::enable() {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            (*it)->enable();
        }
    }

    void Robot::disable() {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            (*it)->disable();
        }
    }

}