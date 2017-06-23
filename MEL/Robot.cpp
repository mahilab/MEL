#include "Robot.h"

Robot::Robot(Daq* daq, uint num_joints) :
    daq_(daq),
    num_joints_(num_joints),
    joint_positions_(double_vec(num_joints, 0.0))
{
    
}

double_vec Robot::get_joint_positions() {
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        joint_positions_[it - joints_.begin()] = it->get_position();
    }
    return joint_positions_;
}

void Robot::set_joint_torques(double_vec joint_torques) {

}