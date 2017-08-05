#pragma once
#include "Robot.h"
#include "AnatomicalJoint.h"

namespace mel {

    class Exo : public Robot {

    public:

        // constructor
        Exo();
        Exo(std::string name);

        // virtual destructor
        virtual ~Exo() {};

        // components
        //std::vector<AnatomicalJoint*> anatomical_joints_; // anatomical joints

        // getters and setters of state variables
        virtual double_vec get_anatomical_joint_positions();
        virtual double_vec get_anatomical_joint_velocities();
        virtual void set_anatomical_joint_torques(double_vec new_torques) {}


    protected:

        double_vec anatomical_joint_positions_;
        double_vec anatomical_joint_velocities_;
        double_vec anatomical_joint_torques_;

    };

}