#include "Robot.h"

namespace mel {

    /// Call RobotJoint::get_position() for each robot joint.
    /// \return vector of doubles containing robot joint positions.
    double_vec Robot::get_joint_positions() {
        for (auto it = joints_.begin(); it != joints_.end(); ++it) {
            joint_positions_[it - joints_.begin()] = (*it)->get_position();
        }
        return joint_positions_;
    }

    /// Call RobotJoint::get_velocity() for each robot joint.
    /// \return vector of doubles containing robot joint velocities.
    double_vec Robot::get_joint_velocities() {
        for (auto it = joints_.begin(); it != joints_.end(); ++it) {
            joint_velocities_[it - joints_.begin()] = (*it)->get_velocity();
        }
        return joint_velocities_;
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