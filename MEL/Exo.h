#pragma once
#include "Robot.h"

namespace mel {

    namespace core {

        class Exo : public Robot {

        public:

            //---------------------------------------------------------------------
            // CONSTRUCTOR(S) / DESTRUCTOR(S)
            //---------------------------------------------------------------------

            Exo();
            Exo(std::string name);
            virtual ~Exo() {};

            //---------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //---------------------------------------------------------------------        

            virtual double_vec get_anatomical_joint_positions() const;
            virtual double get_anatomical_joint_position(int index) const;
            virtual double_vec get_anatomical_joint_velocities() const;
            virtual double get_anatomical_joint_velocity(int index) const;
            virtual void set_anatomical_joint_torques(double_vec new_torques, int error_code = 0) {}
            int error_code_ = 0;

        protected:

            //---------------------------------------------------------------------
            // PROTECTED VARIABLES
            //--------------------------------------------------------------------- 

            double_vec anatomical_joint_positions_;
            double_vec anatomical_joint_velocities_;
            double_vec anatomical_joint_torques_;

        };

    }

}