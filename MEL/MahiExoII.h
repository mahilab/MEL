#pragma once
#include "Exo.h"
#include "util.h"
#include <Eigen\Dense>
#include <Eigen\LU>
#include <Eigen\StdVector>
#include <array>
#include "Motor.h"
#include "Encoder.h"
#include "Integrator.h"
#include "PdController.h"

namespace mel {

    class MahiExoII : public Exo {

    public:

        //-------------------------------------------------------------------------
        // CONFIGURATION / PARAMETERS STRUCTS
        //-------------------------------------------------------------------------

        struct Config {
            std::array<Daq::Do, 5>      enable_;    ///< digital output channels that enable motors
            std::array<Daq::Ao, 5>      command_;   ///< analog output channels that command motors
            std::array<Daq::Encoder, 5> encoder_;   ///< encoder channels that measure motor positions
            std::array<Daq::EncRate, 5> encrate_;   ///< encoder channels that measure motor velocities
        };

        struct Params {
            std::array<double, 5> kt_ = { 0.127, 0.0603, 0.175, 0.175, 0.175 }; ///< motor torque constants [N-m/A]
            std::array<double, 5> motor_continuous_current_limits_ = { 6.0, 3.17, 0.626, 0.626, 0.626 }; ///< motor continuous current limits [A]
            std::array<double, 5> motor_peak_current_limits_ = { 18.0, 18.0, 1.8, 1.8, 1.8 }; ///< motor peak current limits [A]
            std::array<double, 5> motor_i2t_times_ = { 2, 2, 2, 2, 2 }; ///< motor i^2*t times [s]
            std::array<double, 5> eta_ = { 0.42 / 4.5, 0.0662864, INCH2METER*0.23, INCH2METER*0.23, INCH2METER*0.23 }; ///< transmission ratios [inch/inch] or [m]
            std::array<uint32, 5> encoder_res_ = { 2048, 2048, 2048, 2048, 2048 }; ///< encoder resolutions [counts/rev]
            std::array<double, 5> pos_limits_min_ = { -91.5 * DEG2RAD, -99 * DEG2RAD, 0.050, 0.050, 0.050 }; ///< robot joint position limits in negative direction [rad] or [m]
            std::array<double, 5> pos_limits_max_ = { 3 * DEG2RAD, 108 * DEG2RAD, 0.133, 0.133, 0.133 }; ///< robot joint position limits in positive direction [rad] or [m]
            std::array<double, 5> vel_limits_ = { 250 * DEG2RAD, 300 * DEG2RAD, 0.4, 0.4, 0.4 }; ///< robot joint velocity limits [rad/s] or [m/s]
            std::array<double, 5> joint_torque_limits = { 10, 10, 20, 20, 20 }; ///< robot joint torque limits [Nm] or [N]
            std::array<double, 5> amp_gains_ = { 1.8, 1.8, 0.184, 0.184, 0.184 }; ///< motor aplifier gains [A/V]
        };

        //-------------------------------------------------------------------------
        // CONSTRUCTOR / DESTRUCTOR
        //-------------------------------------------------------------------------

        MahiExoII(Config configuration, Params parameters = Params());
        ~MahiExoII() override;

        //-------------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //-------------------------------------------------------------------------

        // kinematics functions
        void update_kinematics();
        void forward_kinematics(double_vec& q_par_in, double_vec& q_ser_out);
        void forward_kinematics(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out);
        void forward_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out);
        void forward_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out, double_vec& qp_dot_out);
        void inverse_kinematics(double_vec& q_ser_in, double_vec& q_par_out);
        void inverse_kinematics(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out);
        void inverse_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out);
        void inverse_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out, double_vec& qp_dot_out);


        // inherited virtual functions from Exo class to be implemented
        void set_anatomical_joint_torques(double_vec new_torques, int error_code = 0) override;

        //-------------------------------------------------------------------------
        // PUBLIC VARIABLES
        //-------------------------------------------------------------------------

        const Config config_;
        const Params params_;

        std::array<PdController, 5> robot_joint_pd_controllers_ = {
            PdController(50.0, 0.25),
            PdController(7.0, 0.06),
            PdController(1000.0, 1.0),
            PdController(1000.0, 1.0),
            PdController(1000.0, 1.0)
        };

        std::array<PdController, 5> anatomical_joint_pd_controllers_ = {
            PdController(50.0, 0.25),
            PdController(7.0, 0.06),
            PdController(25.0, 0.05),
            PdController(30.0, 0.08),
            PdController(0.0, 0.0)
        };

    protected:

        //-------------------------------------------------------------------------
        // PROTECTED VARIABLES
        //-------------------------------------------------------------------------


    private:

        //-------------------------------------------------------------------------
        // PRIVATE VARIABLES
        //-------------------------------------------------------------------------

        // geometric parameters
        const double R_ = 0.1044956;
        const double r_ = 0.05288174521;
        const double a56_ = 0.0268986 - 0.0272820;
        const double alpha5_ = 0.094516665054824;
        const double alpha13_ = DEG2RAD * 5;

        // kinematics variables
        Eigen::VectorXd qp_ = Eigen::VectorXd::Zero(12);
        Eigen::VectorXd qp_0_ = Eigen::VectorXd::Zero(12);
        Eigen::VectorXd q_par_ = Eigen::VectorXd::Zero(3);
        Eigen::VectorXd q_ser_ = Eigen::VectorXd::Zero(3);
        Eigen::VectorXd qp_dot_ = Eigen::VectorXd::Zero(12);
        Eigen::VectorXd q_par_dot_ = Eigen::VectorXd::Zero(3);
        Eigen::VectorXd q_ser_dot_ = Eigen::VectorXd::Zero(3);
        //Eigen::MatrixXd A_ = Eigen::MatrixXd::Zero(3,12);
        //Eigen::VectorXd psi_ = Eigen::VectorXd::Zero(12);
        //Eigen::MatrixXd psi_d_qp_ = Eigen::MatrixXd::Zero(12, 12);
        Eigen::MatrixXd rho_fk_ = Eigen::MatrixXd::Zero(12, 3);
        Eigen::MatrixXd jac_fk_ = Eigen::MatrixXd::Zero(3, 3);
        Eigen::MatrixXd rho_ik_ = Eigen::MatrixXd::Zero(12, 3);
        Eigen::MatrixXd jac_ik_ = Eigen::MatrixXd::Zero(3, 3);
        //Eigen::MatrixXd rho_ = Eigen::MatrixXd::Zero(12, 3);
        Eigen::MatrixXd selector_mat_ = Eigen::MatrixXd::Zero(12, 3);
        const uint32 max_it_ = 10;
        const double tol_ = 1e-12;
        const uint8_vec select_q_par_ = { 3, 4, 5 };
        const uint8_vec select_q_ser_ = { 6, 7, 9 };

        double spec_norm_prev_ = 0; // debugging
        Eigen::VectorXd q_par_prev_ = Eigen::VectorXd::Zero(3); // debugging        


        //-------------------------------------------------------------------------
        // PRIVATE FUNCTIONS
        //-------------------------------------------------------------------------

        // kinematics functions
        void forward_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out);
        void forward_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& jac_fk);
        void forward_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out);
        void forward_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out);
        void inverse_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out);
        void inverse_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out);
        void inverse_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out);
        void inverse_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out, Eigen::VectorXd& qp_dot_out);

        Eigen::VectorXd solve_kinematics(uint8_vec select_q, const Eigen::VectorXd& qs, uint32 max_it, double tol);
        void psi_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qs, const Eigen::VectorXd& qp, Eigen::VectorXd& phi, Eigen::VectorXd& psi);
        void psi_d_qp_update(const Eigen::VectorXd& qp, Eigen::MatrixXd& psi_d_qp);
        void phi_update(const Eigen::VectorXd& qp, Eigen::VectorXd& phi);
        void phi_d_qp_update(const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp);


    };
}