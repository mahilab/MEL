#pragma once
#include "Robot.h"

namespace mel {

    class MahiExoII : public Robot {

    public:

        // constructor
        MahiExoII(Daq* daq, uint_vec ai_channels, uint_vec ao_channels, uint_vec di_channels, uint_vec do_channels, uint_vec enc_channels);

        // destructor
        ~MahiExoII();

        // parameters
        const double R_ = 0.1044956;
        const double r_ = 0.05288174521;
        const double a56_ = 0.0268986 - 0.0272820;
        const double alpha5_ = 0.094516665054824;
        const double alpha13_ = DEG2RAD * 5;

        // references
        Daq* daq_;
        //std::vector<Daq*> daqs_;
        std::vector<Joint*> joints_;
        std::vector<Encoder*> encoders_;
        std::vector<Actuator*> actuators_;

        // state variables
        double_vec psi_;
        double_vec qp_;
        double_vec qs_;
        double_vec psi_d_qp_;
        double_vec joint_positions_;

        //void set_joint_torques(double joint_torques);
        double_vec get_joint_positions();
        void forward_kinematics(double_vec qs, uint max_it, double tol);
        double_vec psi_func(double_vec qp, double_vec qs, double R, double r, double a56, double alpha5, double alpha13);
        double_vec psi_d_qp_func(double_vec qp, double r, double alpha5, double alpha13);

    };
}