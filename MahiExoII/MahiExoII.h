#pragma once
#include "Robot.h"
#include <Eigen\LU>

namespace mel {

    class MahiExoII : public Robot {

    public:

        // constructor
        MahiExoII(Daq* daq, uint32_vec ai_channels, uint32_vec ao_channels, uint32_vec di_channels, uint32_vec do_channels, uint32_vec enc_channels);

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
        double_vec joint_positions_;
        Eigen::VectorXd qp_;
        Eigen::VectorXd qp_prev_;
        Eigen::VectorXd qs_;
        Eigen::VectorXd psi_;
        Eigen::MatrixXd psi_d_qp_;
        Eigen::VectorXd sol_; // testing variable

        //void set_joint_torques(double joint_torques);
        double_vec get_joint_positions();
        void forward_kinematics(Eigen::VectorXd qs, uint32 max_it, double tol);
        void psi_func(Eigen::VectorXd qp, Eigen::VectorXd qs, double R, double r, double a56, double alpha5, double alpha13);
        void psi_d_qp_func(Eigen::VectorXd qp, double r, double alpha5, double alpha13);

    };
}