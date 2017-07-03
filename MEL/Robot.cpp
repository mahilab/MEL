#include "Robot.h"

namespace mel {

    /// Call RobotJoint::get_position() for each robot joint.
    /// \return vector of doubles containing robot joint positions.
    double_vec Robot::get_robot_joint_positions() {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            robot_joint_positions_[it - robot_joints_.begin()] = (*it)->get_position();
        }
        return robot_joint_positions_;
    }

    /// Call RobotJoint::get_velocity() for each robot joint.
    /// \return vector of doubles containing robot joint velocities.
    double_vec Robot::get_robot_joint_velocities() {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            robot_joint_velocities_[it - robot_joints_.begin()] = (*it)->get_velocity();
        }
        return robot_joint_velocities_;
    }

    /// Call RobotJoint::set_torque() for each robot joint.
    /// \param new_torques vector of doubles
    void Robot::set_robot_joint_torques(double_vec new_torques) {
        for (auto it = robot_joints_.begin(); it != robot_joints_.end(); ++it) {
            (*it)->set_torque(new_torques[it-robot_joints_.begin()]);
        }
    }

}