#include <MEL/Core/Robot.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

Robot::Robot(const std::string& name) :
    Device(name)
{}

bool Robot::enable() {
    print("Enabling Robot <" + name_ + "> ... ");
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        it->enable();
    }
    return Device::enable();
}

bool Robot::disable() {
    print("Disabling Robot <" + name_ + "> ... ");
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        it->disable();
    }
    return Device::disable();
}

void Robot::add_joint(const Joint& joint) {
    joints_.push_back(joint);
}

Joint& Robot::get_joint(uint32 joint_number) {
    return joints_[static_cast<std::size_t>(joint_number)];
}

Joint& Robot::operator[](uint32 joint_number) {
    return get_joint(joint_number);
}

std::vector<double> Robot::get_joint_positions() {
    std::vector<double> joint_positions;
    joint_positions.reserve(joints_.size());
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        joint_positions.push_back(it->get_position());
    }
    return joint_positions_ = joint_positions;
}

std::vector<double> Robot::get_joint_velocities() {
    std::vector<double> joint_velocities;
    joint_velocities.reserve(joints_.size());
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        joint_velocities.push_back(it->get_velocity());
    }
    return joint_velocities_ = joint_velocities;
}

void Robot::set_joint_torques(std::vector<double> new_torques) {
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        it->set_torque(new_torques[it - joints_.begin()]);
    }
}

bool Robot::check_all_joint_position_limits() {
    bool exceeded = false;
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        if (it->position_limit_exceeded()) {
            exceeded = true;
        }
    }
    return exceeded;
}

bool Robot::check_all_joint_velocity_limits() {
    bool exceeded = false;
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        if (it->velocity_limit_exceeded()) {
            exceeded = true;
        }
    }
    return exceeded;
}

bool Robot::check_all_joint_torque_limits() {
    bool exceeded = false;
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        if (it->torque_limit_exceeded()) {
            exceeded = true;
        }
    }
    return exceeded;
}

bool Robot::check_all_joint_limits() {
    bool exceeded = false;
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        if (it->any_limit_exceeded()) {
            exceeded = true;
        }
    }
    return exceeded;
}

} // namespace mel
