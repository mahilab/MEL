#include "Robot.h"

namespace mel {

    /// Call RobotJoint::get_position() for each robot joint.
    /// \return vector of doubles containing robot joint positions.
    double_vec Robot::get_joint_positions() {
        double_vec joint_positions;
        joint_positions.reserve(joints_.size());
        for (auto it = joints_.begin(); it != joints_.end(); ++it) {
            joint_positions.push_back((*it)->get_position());
        }
        return joint_positions_ = joint_positions;
    }

    /// Call RobotJoint::get_velocity() for each robot joint.
    /// \return vector of doubles containing robot joint velocities.
    double_vec Robot::get_joint_velocities() {
        double_vec joint_velocities;
        joint_velocities.reserve(joints_.size());
        for (auto it = joints_.begin(); it != joints_.end(); ++it) {
            joint_velocities.push_back((*it)->get_velocity());
        }
        return joint_velocities_ = joint_velocities;
    }

    /// Call RobotJoint::set_torque() for each robot joint.
    /// \param new_torques vector of doubles
    void Robot::set_joint_torques(double_vec new_torques) {
        for (auto it = joints_.begin(); it != joints_.end(); ++it) {
            (*it)->set_torque(new_torques[it-joints_.begin()]);
        }
    }

    void Robot::enable() {
        for (auto it = joints_.begin(); it != joints_.end(); ++it) {
            (*it)->enable();
        }
    }

    void Robot::disable() {
        for (auto it = joints_.begin(); it != joints_.end(); ++it) {
            (*it)->disable();
        }
    }

}