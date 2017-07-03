#pragma once
#include "Exo.h"
#include <Eigen\LU>


namespace mel {

    class MahiExoII : public Exo {

    public:

        MahiExoII(Daq* daq_);


        // inherited virtual functions from Exo class to be implemented
        //double_vec get_anatomical_joint_positions() override;
        //double_vec get_anatomical_joint_velocities() override;
        //void set_anatomical_joint_torques(double_vec new_torques) override;
        

    private:
        /*
        // parameters (make these static?)
        const double R_ = 0.1044956;
        const double r_ = 0.05288174521;
        const double a56_ = 0.0268986 - 0.0272820;
        const double alpha5_ = 0.094516665054824;
        const double alpha13_ = DEG2RAD * 5;

        // state variables
        Eigen::VectorXd qp_;
        Eigen::VectorXd qp_prev_;
        Eigen::VectorXd qp_dot_;
        Eigen::VectorXd qs_;
        Eigen::VectorXd qs_dot_;
        Eigen::VectorXd psi_;
        Eigen::MatrixXd psi_d_qp_;
        Eigen::VectorXd sol_; // testing variable

        void psi_func(Eigen::VectorXd qp, Eigen::VectorXd qs, double R, double r, double a56, double alpha5, double alpha13);
        void psi_d_qp_func(Eigen::VectorXd qp, double r, double alpha5, double alpha13);
        void forward_kinematics(Eigen::VectorXd qs, uint32 max_it, double tol);
        void forward_kinematics_velocity(Eigen::VectorXd qs_dot);
        */
    };
}