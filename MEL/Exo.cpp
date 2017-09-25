#include "Exo.h"

namespace mel {

    namespace core {

        Exo::Exo() :
            Robot()
        { }

        Exo::Exo(std::string name) :
            Robot(name)
        { }

        double_vec Exo::get_anatomical_joint_positions() const {
            return anatomical_joint_positions_;
        }

        double Exo::get_anatomical_joint_position(int index) const {
            return anatomical_joint_positions_[index];
        }

        double_vec Exo::get_anatomical_joint_velocities() const {
            return anatomical_joint_velocities_;
        }

        double Exo::get_anatomical_joint_velocity(int index) const {
            return anatomical_joint_velocities_[index];
        }

    }

}