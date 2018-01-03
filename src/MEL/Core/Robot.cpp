#include <MEL/Core/Robot.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

bool Robot::enable() {
    print("Enabling Robot <" + name_ + "> ... ", false);
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        (*it)->enable();
    }
    print("Done");
    return Device::enable();
}

bool Robot::disable() {
    print("Disabling Robot <" + name_ + "> ... ", false);
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        (*it)->disable();
    }
    print("Done");
    return Device::disable();
}

/// Call RobotJoint::get_position() for each robot joint.
/// \return vector of doubles containing robot joint positions.
std::vector<double> Robot::get_joint_positions() {
    std::vector<double> joint_positions;
    joint_positions.reserve(joints_.size());
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        joint_positions.push_back((*it)->get_position());
    }
    return joint_positions_ = joint_positions;
}

/// Call RobotJoint::get_velocity() for each robot joint.
/// \return vector of doubles containing robot joint velocities.
std::vector<double> Robot::get_joint_velocities() {
    std::vector<double> joint_velocities;
    joint_velocities.reserve(joints_.size());
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        joint_velocities.push_back((*it)->get_velocity());
    }
    return joint_velocities_ = joint_velocities;
}

/// Call RobotJoint::set_torque() for each robot joint.
/// \param new_torques vector of doubles
void Robot::set_joint_torques(std::vector<double> new_torques) {
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

} // namespace mel
