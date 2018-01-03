#pragma once
#include <MEL/Core/Robot.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Exo : public Robot {

public:

    Exo();

    Exo(std::string name);

    virtual ~Exo() {};

    virtual std::vector<double> get_anatomical_joint_positions() const;

    virtual double get_anatomical_joint_position(int index) const;

    virtual std::vector<double> get_anatomical_joint_velocities() const;

    virtual double get_anatomical_joint_velocity(int index) const;

    virtual void set_anatomical_joint_torques(std::vector<double> new_torques) {}

protected:

    std::vector<double> anatomical_joint_positions_;
    std::vector<double> anatomical_joint_velocities_;
    std::vector<double> anatomical_joint_torques_;

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
