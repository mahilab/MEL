#include "Robot.h"

namespace mel {

    namespace core {

        void Robot::enable() {
            enabled_ = true;
            util::print("Enabling Robot <" + name_ + "> ... ", false);
            for (auto it = joints_.begin(); it != joints_.end(); ++it) {
                (*it)->enable();
            }
            util::print("Done");
        }

        void Robot::disable() {
            enabled_ = false;
            util::print("Disabling Robot <" + name_ + "> ... ", false);
            for (auto it = joints_.begin(); it != joints_.end(); ++it) {
                (*it)->disable();
            }
            util::print("Done");
        }

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
                (*it)->set_torque(new_torques[it - joints_.begin()]);
            }
        }

        bool Robot::check_all_joint_position_limits() {
            bool exceeded = false;
            for (auto it = joints_.begin(); it != joints_.end(); ++it) {
                if ((*it)->check_position_limits()) {
                    exceeded = true;
                }
            }
            return exceeded;
        }

        bool Robot::check_all_joint_velocity_limits() {
            bool exceeded = false;
            for (auto it = joints_.begin(); it != joints_.end(); ++it) {
                if ((*it)->check_velocity_limit()) {
                    exceeded = true;
                }
            }
            return exceeded;
        }

        bool Robot::check_all_joint_torque_limits() {
            bool exceeded = false;
            for (auto it = joints_.begin(); it != joints_.end(); ++it) {
                if ((*it)->check_torque_limit()) {
                    exceeded = true;
                }
            }
            return exceeded;
        }

        bool Robot::check_all_joint_limits() {
            bool exceeded = false;
            for (auto it = joints_.begin(); it != joints_.end(); ++it) {
                if ((*it)->check_all_limits()) {
                    exceeded = true;
                }
            }
            return exceeded;
        }

    }

}