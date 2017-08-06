#include "Exo.h"

namespace mel {

    Exo::Exo() :
        Robot(),
        anatomical_joint_positions_({ 0,0,0,0 }),
        anatomical_joint_velocities_({ 0,0,0,0 }),
        anatomical_joint_torques_({ 0,0,0,0 })
    { }

    Exo::Exo(std::string name) :
        Robot(name),
        anatomical_joint_positions_({ 0,0,0,0 }),
        anatomical_joint_velocities_({ 0,0,0,0 }),
        anatomical_joint_torques_({ 0,0,0,0 })
    { }



    double_vec Exo::get_anatomical_joint_positions() {
        return anatomical_joint_positions_;
    }

    double Exo::get_anatomical_joint_position(int index) {
        return anatomical_joint_positions_[index];
    }

    double_vec Exo::get_anatomical_joint_velocities() {
        return anatomical_joint_velocities_;
    }

    double Exo::get_anatomical_joint_velocity(int index) {
        return anatomical_joint_velocities_[index];
    }

}