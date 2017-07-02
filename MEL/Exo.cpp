#include "Exo.h"

namespace mel {

    Exo::Exo() :
        anatomical_joint_positions_({ 0,0,0,0 }),
        anatomical_joint_velocities_({ 0,0,0,0 }),
        anatomical_joint_torques_({ 0,0,0,0 })
    {

    }

    double_vec Exo::get_anatomical_joint_positions() {
        return anatomical_joint_positions_;
    }

    double_vec Exo::get_anatomical_joint_velocities() {
        return anatomical_joint_velocities_;
    }


}