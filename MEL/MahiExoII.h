#pragma once
#include "Exo.h"
#include "mel_util.h"
#include "mel_math.h"
#include <Eigen\Dense>
#include <Eigen\LU>
#include <Eigen\StdVector>
#include <array>
#include "Motor.h"
#include "Encoder.h"
#include "Integrator.h"
#include "PdController.h"

namespace mel {

    namespace exo {

        class MahiExoII : public core::Exo {

        public:

            //-------------------------------------------------------------------------
            // CONFIGURATION / PARAMETERS STRUCTS
            //-------------------------------------------------------------------------

            struct Config {
                std::array<core::Daq::Do, 5>      enable_;    ///< digital output channels that enable motors
                std::array<core::Daq::Ao, 5>      command_;   ///< analog output channels that command motors
                std::array<core::Daq::Encoder, 5> encoder_;   ///< encoder channels that measure motor positions
                std::array<core::Daq::EncRate, 5> encrate_;   ///< encoder channels that measure motor velocities
            };

            struct Params {
                std::array<double, 5> kt_ = { 0.127, 0.0603, 0.175, 0.175, 0.175 }; ///< motor torque constants [N-m/A]
                std::array<double, 5> motor_hard_current_limits_ = { 8.0, 4.0, 0.75, 0.75, 0.75 }; ///< motor continuous current limits [A]
                std::array<double, 5> motor_continuous_current_limits_ = { 6.0, 3.17, 0.626, 0.626, 0.626 }; ///< motor continuous current limits [A]
                std::array<double, 5> motor_peak_current_limits_ = { 18.0, 18.0, 1.8, 1.8, 1.8 }; ///< motor peak current limits [A]
                std::array<double, 5> motor_i2t_times_ = { 2, 2, 2, 2, 2 }; ///< motor i^2*t times [s]
                std::array<double, 5> eta_ = { 0.42 / 4.5, 0.0662864, math::INCH2METER*0.23, math::INCH2METER*0.23, math::INCH2METER*0.23 }; ///< transmission ratios [inch/inch] or [m]
                std::array<uint32, 5> encoder_res_ = { 2048, 2048, 2048, 2048, 2048 }; ///< encoder resolutions [counts/rev]
                std::array<double, 5> pos_limits_min_ = { -91.5 * math::DEG2RAD, -99 * math::DEG2RAD, 0.050, 0.050, 0.050 }; ///< robot joint position limits in negative direction [rad] or [m]
                std::array<double, 5> pos_limits_max_ = { 3 * math::DEG2RAD, 108 * math::DEG2RAD, 0.133, 0.133, 0.133 }; ///< robot joint position limits in positive direction [rad] or [m]
                std::array<double, 5> vel_limits_ = { 250 * math::DEG2RAD, 300 * math::DEG2RAD, 0.4, 0.4, 0.4 }; ///< robot joint velocity limits [rad/s] or [m/s]
                std::array<double, 5> joint_torque_limits = { 10, 10, 50, 50, 50 }; ///< robot joint torque limits [Nm] or [N]
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

            // update robot kinematics from encoder readings
            void update_kinematics();

            double_vec get_wrist_parallel_positions() const;
            double_vec get_wrist_serial_positions() const;

            // inherited virtual functions from Exo class to be implemented
            void set_anatomical_joint_torques(double_vec new_torques, int error_code = 0) override;

            // forward kinematics utility functions
            void forward_kinematics(double_vec& q_par_in, double_vec& q_ser_out) const;
            void forward_kinematics(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out) const;
            void forward_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out) const;
            void forward_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out, double_vec& qp_dot_out) const;
            
            // inverse kinematics utility functions
            void inverse_kinematics(double_vec& q_ser_in, double_vec& q_par_out) const;
            void inverse_kinematics(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out) const;
            void inverse_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out) const;
            void inverse_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out, double_vec& qp_dot_out) const;

            

            //-------------------------------------------------------------------------
            // PUBLIC VARIABLES
            //-------------------------------------------------------------------------

            const Config config_;
            const Params params_;

            static const int N_aj_; // number of anatomical joints
            static const int N_rj_; // number of robotic joints

            // PD Control
            double elbow_P_ = 80.0; //80.0; // tuned 9/11/2017
            double elbow_D_ = 1.25; // 1.50; // tuned 9/11/2017
            double forearm_P_ = 25.0;
            double forearm_D_ = 0.20;
            double prismatic_P_ = 2200.0; // tuned 9//12/2017
            double prismatic_D_ = 30.0; // tuned 9//12/2017
            double wrist_fe_P_ = 25.0;
            double wrist_fe_D_ = 0.05;
            double wrist_ru_P_ = 30.0;
            double wrist_ru_D_ = 0.08;

            std::array<core::PdController, 5> robot_joint_pd_controllers_ = {
                core::PdController(elbow_P_, elbow_D_),
                core::PdController(forearm_P_, forearm_D_),
                core::PdController(prismatic_P_, prismatic_D_),
                core::PdController(prismatic_P_, prismatic_D_),
                core::PdController(prismatic_P_, prismatic_D_)
            };

            std::array<core::PdController, 5> anatomical_joint_pd_controllers_ = {
                core::PdController(elbow_P_, elbow_D_),
                core::PdController(forearm_P_, forearm_D_),
                core::PdController(wrist_fe_P_, wrist_fe_D_),
                core::PdController(wrist_ru_P_, wrist_ru_D_),
                core::PdController(0.0, 0.0)
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
            static const double R_;
            static const double r_;
            static const double a56_;
            static const double alpha5_;
            static const double alpha13_;

            // continuously updated kinematics variables
            Eigen::VectorXd qp_ = Eigen::VectorXd::Zero(12);
            Eigen::VectorXd q_par_ = Eigen::VectorXd::Zero(3);
            Eigen::VectorXd q_ser_ = Eigen::VectorXd::Zero(3);
            Eigen::VectorXd qp_dot_ = Eigen::VectorXd::Zero(12);
            Eigen::VectorXd q_par_dot_ = Eigen::VectorXd::Zero(3);
            Eigen::VectorXd q_ser_dot_ = Eigen::VectorXd::Zero(3);
            Eigen::MatrixXd rho_fk_ = Eigen::MatrixXd::Zero(12, 3);
            Eigen::MatrixXd jac_fk_ = Eigen::MatrixXd::Zero(3, 3);
            //Eigen::MatrixXd rho_ik_ = Eigen::MatrixXd::Zero(12, 3);
            //Eigen::MatrixXd jac_ik_ = Eigen::MatrixXd::Zero(3, 3);

            // kinematics solver setup variables
            const uint32 max_it_ = 10;
            const double tol_ = 1e-12;
            const uint8_vec select_q_par_ = { 3, 4, 5 };
            const uint8_vec select_q_ser_ = { 6, 7, 9 };

            double spec_norm_prev_ = 0; // debugging
            Eigen::VectorXd q_par_prev_ = Eigen::VectorXd::Zero(3); // debugging        

            


            //-------------------------------------------------------------------------
            // PRIVATE FUNCTIONS
            //-------------------------------------------------------------------------

            // forward kinematics functions
            void forward_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out) const;
            void forward_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out) const;
            void forward_kinematics(const Eigen::VectorXd& q_par_in, Eigen::MatrixXd& jac_fk) const;
            void forward_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::MatrixXd& jac_fk) const;
            void forward_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_fk, Eigen::MatrixXd& jac_fk) const;
            void forward_kinematics_velocity(const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const;
            void forward_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const;
            void forward_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::MatrixXd& jac_fk, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const;
            void forward_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out) const;
            void forward_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_fk, Eigen::MatrixXd& jac_fk, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out) const;
            
            // inverse kinematics functions
            void inverse_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out) const;
            void inverse_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out) const;
            void inverse_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::MatrixXd& jac_ik) const;
            void inverse_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::MatrixXd& jac_ik) const;
            void inverse_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_ik, Eigen::MatrixXd& jac_ik) const;
            void inverse_kinematics_velocity(const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const;
            void inverse_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const;
            void inverse_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::MatrixXd& jac_ik, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const;
            void inverse_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out, Eigen::VectorXd& qp_dot_out) const;
            void inverse_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_ik, Eigen::MatrixXd& jac_ik, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out, Eigen::VectorXd& qp_dot_out) const;

            // general kinematics functions
            void solve_kinematics(uint8_vec select_q, const Eigen::VectorXd& qs, Eigen::VectorXd& qp, Eigen::MatrixXd& rho, uint32 max_it, double tol) const;
            void psi_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qs, const Eigen::VectorXd& qp, Eigen::VectorXd& phi, Eigen::VectorXd& psi) const;
            void psi_d_qp_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp, Eigen::MatrixXd& psi_d_qp) const;
            void phi_update(const Eigen::VectorXd& qp, Eigen::VectorXd& phi) const;
            void phi_d_qp_update(const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp) const;

        };

    }

}