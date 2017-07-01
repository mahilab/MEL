#pragma once
#include "Robot.h"
#include "AnatomicalJoint.h"

namespace mel {

    class Exo : public Robot {

    public:

        // constructor
        Exo();

        // components
        //std::vector<AnatomicalJoint*> anatomical_joints_; // anatomical joints

        // getters and setters of state variables
        virtual double_vec get_anatomical_joint_positions();
        virtual double_vec get_anatomical_joint_velocities();
        virtual void set_anatomical_joint_torques(double_vec new_torques);

        /*
        virtual double get_elbow_joint_position();
        virtual double get_radioulnar_joint_position();
        virtual double get_wrist_fe_joint_position();
        virtual double get_wrist_ru_joint_position();
        
        virtual double get_elbow_joint_velocity();
        virtual double get_radioulnar_joint_velocity();
        virtual double get_wrist_fe_joint_velocity();
        virtual double get_wrist_ru_joint_velocity();

        virtual void set_elbow_joint_torque(double new_torque);
        virtual void set_radioulnar_joint_torque(double new_torque);
        virtual void set_wrist_fe_joint_torque(double new_torque);
        virtual void set_wrist_ru_joint_torque(double new_torque);
        */

    protected:

        double_vec anatomical_joint_positions_;
        double_vec anatomical_joint_velocities_;
        double_vec anatomical_joint_torques_;

    };

}