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

            static const int N_aj_ = 5; // number of anatomical joints
            static const int N_rj_ = 5; // number of robotic joints

            static const int N_qp_ = 12; // number of rps dependent DoF 
            static const int N_qs_ = 3; // number of rps independent DoF

            //-------------------------------------------------------------------------
            // CONFIGURATION / PARAMETERS STRUCTS
            //-------------------------------------------------------------------------

            struct Config {
                std::array<core::Daq::Do, N_rj_>      enable_;    ///< digital output channels that enable motors
                std::array<core::Daq::Ao, N_rj_>      command_;   ///< analog output channels that command motors
                std::array<core::Daq::Encoder, N_rj_> encoder_;   ///< encoder channels that measure motor positions
                std::array<core::Daq::EncRate, N_rj_> encrate_;   ///< encoder channels that measure motor velocities
            };

            struct Params {
                std::array<double, N_rj_> kt_ = { 0.127, 0.0603, 0.175, 0.175, 0.175 }; ///< motor torque constants [N-m/A]
                std::array<double, N_rj_> motor_hard_current_limits_ = { 8.0, 4.0, 0.75, 0.75, 0.75 }; ///< motor continuous current limits [A]
                std::array<double, N_rj_> motor_continuous_current_limits_ = { 6.0, 3.17, 0.626, 0.626, 0.626 }; ///< motor continuous current limits [A]
                std::array<double, N_rj_> motor_peak_current_limits_ = { 18.0, 18.0, 1.8, 1.8, 1.8 }; ///< motor peak current limits [A]
                std::array<double, N_rj_> motor_i2t_times_ = { 2, 2, 2, 2, 2 }; ///< motor i^2*t times [s]
                std::array<double, N_rj_> eta_ = { 0.42 / 4.5, 0.0662864, math::INCH2METER*0.23, math::INCH2METER*0.23, math::INCH2METER*0.23 }; ///< transmission ratios [inch/inch] or [m]
                std::array<uint32, N_rj_> encoder_res_ = { 2048, 2048, 2048, 2048, 2048 }; ///< encoder resolutions [counts/rev]
                std::array<double, N_rj_> pos_limits_min_ = { -91.5 * math::DEG2RAD, -99 * math::DEG2RAD, 0.050, 0.050, 0.050 }; ///< robot joint position limits in negative direction [rad] or [m]
                std::array<double, N_rj_> pos_limits_max_ = { 3 * math::DEG2RAD, 108 * math::DEG2RAD, 0.133, 0.133, 0.133 }; ///< robot joint position limits in positive direction [rad] or [m]
                std::array<double, N_rj_> vel_limits_ = { 250 * math::DEG2RAD, 300 * math::DEG2RAD, 0.4, 0.4, 0.4 }; ///< robot joint velocity limits [rad/s] or [m/s]
                std::array<double, N_rj_> joint_torque_limits = { 10, 10, 50, 50, 50 }; ///< robot joint torque limits [Nm] or [N]
                std::array<double, N_rj_> amp_gains_ = { 1.8, 1.8, 0.184, 0.184, 0.184 }; ///< motor aplifier gains [A/V]
            };

            //-------------------------------------------------------------------------
            // CONSTRUCTOR / DESTRUCTOR
            //-------------------------------------------------------------------------

            MahiExoII(Config configuration, Params parameters = Params());
            ~MahiExoII() override;

            //-------------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //-------------------------------------------------------------------------

            void disable() override;

            class SmoothReferenceTrajectory {

            public:

                SmoothReferenceTrajectory() {};

                SmoothReferenceTrajectory(double_vec speed) :
                    speed_(speed),
                    n_dof_(speed.size()),
                    ref_init_(false)
                {}

                SmoothReferenceTrajectory(double_vec speed, double_vec ref_pos) :
                    speed_(speed),
                    n_dof_(speed.size()),
                    ref_(ref_pos),
                    ref_init_(true)
                {}

                void start(double_vec current_pos, double current_time);
                void start(double_vec ref_pos, double_vec current_pos, double current_time);
                void set_ref(double_vec ref_pos, double current_time);
                double calculate_smooth_ref(int dof, double current_time);
                void stop();

                bool is_started() { return started_; };

            private:

                size_t n_dof_;
                double start_time_ = 0.0;
                double_vec speed_;
                bool started_ = false;
                double_vec ref_;
                bool ref_init_;
                double_vec prev_ref_;

            };


            // rps position control functions
            void set_rps_control_mode(int mode);
            void set_rps_backdrive(bool backdrive = false);         
            void set_rps_pos_ctrl_torques(SmoothReferenceTrajectory& rps_ref, double current_time);

            // rps position controllers
            SmoothReferenceTrajectory rps_init_par_ref_;
            SmoothReferenceTrajectory rps_par_ref_;
            SmoothReferenceTrajectory rps_ser_ref_;

            // full robot position control functions
            void set_elbow_backdrive(bool backdrive = false);
            void set_forearm_backdrive(bool backdrive = false);
            double_vec set_anat_pos_ctrl_torques(SmoothReferenceTrajectory& anat_ref, double current_time);

            // full robot position controllers
            SmoothReferenceTrajectory robot_ref_;
            SmoothReferenceTrajectory anat_ref_;

            // update robot kinematics from encoder readings
            void update_kinematics();

            // read wrist kinematics after using update_kinematics
            double_vec get_wrist_parallel_positions() const;
            double_vec get_wrist_serial_positions() const;

            // send torque commands to the exo
            void set_anatomical_joint_torques(double_vec new_torques, int error_code = 0) override;
            void set_rps_par_torques(double_vec& tau_par);
            void set_rps_ser_torques(double_vec& tau_ser);

            // forward kinematics utility functions
            void forward_rps_kinematics(double_vec& q_par_in, double_vec& q_ser_out) const;
            void forward_rps_kinematics(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out) const;
            void forward_rps_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out) const;
            void forward_rps_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out, double_vec& qp_dot_out) const;
            
            // inverse kinematics utility functions
            void inverse_rps_kinematics(double_vec& q_ser_in, double_vec& q_par_out) const;
            void inverse_rps_kinematics(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out) const;
            void inverse_rps_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out) const;
            void inverse_rps_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out, double_vec& qp_dot_out) const;

            
            //-----------------------------------------------------------------------------
            // PUBLIC UTILITY FUNCTIONS
            //-----------------------------------------------------------------------------

            bool check_rps_init(bool print_output = false) const;
            bool check_goal_rps_par_pos(double_vec goal_rps_par_pos, char_vec check_dof, bool print_output = false) const;
            bool check_goal_rps_ser_pos(double_vec goal_rps_ser_pos, char_vec check_dof, bool print_output = false) const;
            bool check_goal_anat_pos(double_vec goal_anat_pos, char_vec check_dof, bool print_output = false) const;

            //-------------------------------------------------------------------------
            // PUBLIC VARIABLES
            //-------------------------------------------------------------------------

            const Config config_;
            const Params params_;

            // PD Control
            double elbow_P_ = 100.0; // tuned 9/13/2017
            double elbow_D_ = 1.25; // tuned 9/13/2017
            double forearm_P_ = 28.0; // tuned 9/13/2017
            double forearm_D_ = 0.20; // tuned 9/13/2017
            double prismatic_P_ = 2200.0; // tuned 9//12/2017
            double prismatic_D_ = 30.0; // tuned 9//12/2017
            double wrist_fe_P_ = 15.0; //25.0;
            double wrist_fe_D_ = 0.01; //0.05;
            double wrist_ru_P_ = 15.0; //30.0;
            double wrist_ru_D_ = 0.01; //0.08;

            std::array<core::PdController, N_rj_> robot_joint_pd_controllers_ = {
                core::PdController(elbow_P_, elbow_D_),
                core::PdController(forearm_P_, forearm_D_),
                core::PdController(prismatic_P_, prismatic_D_),
                core::PdController(prismatic_P_, prismatic_D_),
                core::PdController(prismatic_P_, prismatic_D_)
            };

            std::array<core::PdController, N_aj_> anatomical_joint_pd_controllers_ = {
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

            // rps position control
            int rps_control_mode_ = 0; // 0 = robot joint space (parallel), 1 = anatomical joint space (serial)
            bool rps_backdrive_ = false; // true = rps is backdrivable, false = rps is active
            double rps_init_err_tol_ = 0.003; // [m]
            double_vec rps_par_goal_err_tol_ = double_vec(N_qs_, 0.003);
            double_vec rps_ser_goal_err_tol_ = { 2.0 * math::DEG2RAD, 2.0 * math::DEG2RAD, 0.005 };
            const double_vec rps_init_pos_ = { 0.10, 0.10, 0.10 };
            const double_vec rps_par_joint_speed_ = { 0.015, 0.015, 0.015 };
            const double_vec rps_ser_joint_speed_ = { 0.125, 0.125, 0.015 };


            // full robot position control
            bool elbow_backdrive_ = false; // true = elbow is backdrivable, false = elbow is active
            bool forearm_backdrive_ = false; // true = forearm is backdrivable, false = forearm is active
            double_vec anat_goal_err_tol_ = { 1.0 * math::DEG2RAD, 1.0 * math::DEG2RAD, 2.0 * math::DEG2RAD, 2.0 * math::DEG2RAD, 0.005 };
            const double_vec robot_joint_speed_ = { 0.25, 0.25, 0.015, 0.015, 0.015 };
            const double_vec anat_joint_speed_ = { 0.25, 0.25, 0.125, 0.125, 0.015 }; // constant speed at which anatomical joint reference trajectories are interpolated


            // geometric parameters
            static const double R_;
            static const double r_;
            static const double a56_;
            static const double alpha5_;
            static const double alpha13_;

            // continuously updated kinematics variables
            Eigen::VectorXd qp_ = Eigen::VectorXd::Zero(N_qp_);
            Eigen::VectorXd q_par_ = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd q_ser_ = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd qp_dot_ = Eigen::VectorXd::Zero(N_qp_);
            Eigen::VectorXd q_par_dot_ = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd q_ser_dot_ = Eigen::VectorXd::Zero(N_qs_);
            Eigen::MatrixXd rho_fk_ = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
            Eigen::MatrixXd jac_fk_ = Eigen::MatrixXd::Zero(N_qs_, N_qs_);

            

            // kinematics solver setup variables
            const uint32 max_it_ = 10;
            const double tol_ = 1e-12;
            const uint8_vec select_q_par_ = { 3, 4, 5 };
            const uint8_vec select_q_ser_ = { 6, 7, 9 };

            double spec_norm_prev_ = 0; // debugging
            Eigen::VectorXd q_par_prev_ = Eigen::VectorXd::Zero(N_qs_); // debugging        

            


            //-------------------------------------------------------------------------
            // PRIVATE FUNCTIONS
            //-------------------------------------------------------------------------

            // solving for static equilibrium joint torques           
            void solve_static_rps_torques(uint8_vec select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s) const;
            void solve_static_rps_torques(uint8_vec select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s, Eigen::VectorXd& tau_p) const;


            // forward kinematics functions
            void forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out) const;
            void forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out) const;
            void forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::MatrixXd& jac_fk) const;
            void forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::MatrixXd& jac_fk) const;
            void forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_fk, Eigen::MatrixXd& jac_fk) const;
            void forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const;
            void forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const;
            void forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::MatrixXd& jac_fk, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const;
            void forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out) const;
            void forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_fk, Eigen::MatrixXd& jac_fk, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out) const;
            
            // inverse kinematics functions
            void inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out) const;
            void inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out) const;
            void inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::MatrixXd& jac_ik) const;
            void inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::MatrixXd& jac_ik) const;
            void inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_ik, Eigen::MatrixXd& jac_ik) const;
            void inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const;
            void inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const;
            void inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::MatrixXd& jac_ik, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const;
            void inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out, Eigen::VectorXd& qp_dot_out) const;
            void inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_ik, Eigen::MatrixXd& jac_ik, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out, Eigen::VectorXd& qp_dot_out) const;

            // general kinematics functions
            void solve_rps_kinematics(uint8_vec select_q, const Eigen::VectorXd& qs, Eigen::VectorXd& qp, Eigen::MatrixXd& rho, Eigen::MatrixXd& rho_s, uint32 max_it, double tol) const;
            void generate_rho(uint8_vec select_q, const Eigen::VectorXd& qp, Eigen::MatrixXd& rho) const;
            void psi_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qs, const Eigen::VectorXd& qp, Eigen::VectorXd& phi, Eigen::VectorXd& psi) const;
            void psi_d_qp_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp, Eigen::MatrixXd& psi_d_qp) const;
            void phi_update(const Eigen::VectorXd& qp, Eigen::VectorXd& phi) const;
            void phi_d_qp_update(const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp) const;
            uint8_vec select_q_invert(uint8_vec select_q) const;

            //-----------------------------------------------------------------------------
            // PRIVATE UTILITY FUNCTIONS
            //-----------------------------------------------------------------------------

            bool check_goal_pos(double_vec goal_pos, double_vec current_pos, char_vec check_joint, double_vec error_tol, bool print_output = false) const;


        };

    }

}