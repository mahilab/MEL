// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_MAHIEXOII_HPP
#define MEL_MAHIEXOII_HPP

#include "MEL/Exoskeletons/MahiExoII/MeiiConfiguration.hpp"
#include <MEL/Exoskeletons/MahiExoII/MeiiParameters.hpp>
#include "MEL/Core/Exo.hpp"
#include "MEL/Core/Motor.hpp"
#include "MEL/Core/PdController.hpp"
#include "MEL/Utility/Time.hpp"
#include <array>
#include <vector>
#include <atomic>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/StdVector>


namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class MahiExoII : public Exo {

public:

    // Static public variables 
    static const int N_aj_ = 5; ///< number of anatomical joints
    static const int N_rj_ = 5; ///< number of robotic joints

    static const int N_qp_ = 12; ///< number of rps dependent DoF 
    static const int N_qs_ = 3; ///< number of rps independent DoF

    /// Constructor
    MahiExoII(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());

    /// Destructor
    ~MahiExoII() override;

    /// Manually zero the encoders
    void calibrate(volatile std::atomic<bool>& stop_flag);

    /// Disables the robot and stops all smooth reference trajectories
    bool disable() override;

    /// vector of motors belonging to MahiExoII
    std::vector<Motor> motors_;

    /// Class that generates smooth reference trajectories that can be updated in real time
    class SmoothReferenceTrajectory {

    public:

        SmoothReferenceTrajectory() {};

        SmoothReferenceTrajectory(std::vector<double>  speed) :
            speed_(speed),
            n_dof_(speed.size()),
            ref_init_(false)
        {}

        SmoothReferenceTrajectory(std::vector<double>  speed, std::vector<double> ref_pos) :
            speed_(speed),
            n_dof_(speed.size()),
            ref_(ref_pos),
            ref_init_(true)
        {}

        void start(std::vector<double> current_pos, Time current_time);
        void start(std::vector<double> ref_pos, std::vector<double> current_pos, Time current_time);
        void set_ref(std::vector<double> ref_pos, Time current_time);
        double calculate_smooth_ref(int dof, Time current_time);
        void stop();

        bool is_started() { return started_; };

    private:

        size_t n_dof_;
        Time start_time_ = seconds(0.0);
        std::vector<double> speed_;
        bool started_ = false;
        std::vector<double> ref_;
        bool ref_init_;
        std::vector<double> prev_ref_;

    };


    // rps position control functions
    void set_rps_control_mode(int mode);
    void set_rps_backdrive(bool backdrive = false);         
    std::vector<double> set_rps_pos_ctrl_torques(SmoothReferenceTrajectory& rps_ref, Time current_time);

    // rps position controllers
    SmoothReferenceTrajectory rps_init_par_ref_;
    SmoothReferenceTrajectory rps_par_ref_;
    SmoothReferenceTrajectory rps_ser_ref_;

    // full robot position control functions
    void set_elbow_backdrive(bool backdrive = false);
    void set_forearm_backdrive(bool backdrive = false);
    std::vector<double> set_anat_pos_ctrl_torques(SmoothReferenceTrajectory& anat_ref, Time current_time);

    // full robot position controllers
    SmoothReferenceTrajectory robot_ref_;
    SmoothReferenceTrajectory anat_ref_;

    // update robot kinematics from encoder readings
    void update_kinematics();

    // read wrist kinematics after using update_kinematics
    std::vector<double> get_wrist_parallel_positions() const;
    std::vector<double> get_wrist_serial_positions() const;

    // send torque commands to the exo
    void set_anatomical_joint_torques(std::vector<double> new_torques) override;
    void set_rps_par_torques(std::vector<double>& tau_par);
    void set_rps_ser_torques(std::vector<double>& tau_ser);

    // forward kinematics utility functions
    void forward_rps_kinematics(std::vector<double>& q_par_in, std::vector<double>& q_ser_out) const;
    void forward_rps_kinematics(std::vector<double>& q_par_in, std::vector<double>& q_ser_out, std::vector<double>& qp_out) const;
    void forward_rps_kinematics_velocity(std::vector<double>& q_par_in, std::vector<double>& q_ser_out, std::vector<double>& q_par_dot_in, std::vector<double>& q_ser_dot_out) const;
    void forward_rps_kinematics_velocity(std::vector<double>& q_par_in, std::vector<double>& q_ser_out, std::vector<double>& qp_out, std::vector<double>& q_par_dot_in, std::vector<double>& q_ser_dot_out, std::vector<double>& qp_dot_out) const;
            
    // inverse kinematics utility functions
    void inverse_rps_kinematics(std::vector<double>& q_ser_in, std::vector<double>& q_par_out) const;
    void inverse_rps_kinematics(std::vector<double>& q_ser_in, std::vector<double>& q_par_out, std::vector<double>& qp_out) const;
    void inverse_rps_kinematics_velocity(std::vector<double>& q_ser_in, std::vector<double>& q_par_out, std::vector<double>& q_ser_dot_in, std::vector<double>& q_par_dot_out) const;
    void inverse_rps_kinematics_velocity(std::vector<double>& q_ser_in, std::vector<double>& q_par_out, std::vector<double>& qp_out, std::vector<double>& q_ser_dot_in, std::vector<double>& q_par_dot_out, std::vector<double>& qp_dot_out) const;
         
    // PUBLIC UTILITY FUNCTIONS
    bool check_rps_init(bool print_output = false) const;
    bool check_goal_rps_par_pos(std::vector<double> goal_rps_par_pos, std::vector<char> check_dof, bool print_output = false) const;
    bool check_goal_rps_ser_pos(std::vector<double> goal_rps_ser_pos, std::vector<char> check_dof, bool print_output = false) const;
    bool check_goal_anat_pos(std::vector<double> goal_anat_pos, std::vector<char> check_dof, bool print_output = false) const;
    bool check_neutral_anat_pos(std::vector<double> goal_anat_pos, std::vector<char> check_dof, bool print_output = false) const;

    // PUBLIC DATA LOGGING FUNCTIONS
    //void init_robot_log();
    //void log_robot_row(double time);
    //void save_and_clear_robot_log(std::string filename, std::string directory, bool timestamp = true);


    MeiiConfiguration config_;
    const MeiiParameters params_;

    // PD Control
    double elbow_P_ = 100.0; // tuned 9/13/2017
    double elbow_D_ = 1.25; // tuned 9/13/2017
    double forearm_P_ = 28.0; // tuned 9/13/2017
    double forearm_D_ = 0.20; // tuned 9/13/2017
    double prismatic_P_ = 2200.0; // tuned 9//12/2017
    double prismatic_D_ = 30.0; // tuned 9//12/2017
    double wrist_fe_P_ = 15.0; // previously tried 25.0;
    double wrist_fe_D_ = 0.01; // previously tried 0.05;
    double wrist_ru_P_ = 15.0; // previously tried 30.0;
    double wrist_ru_D_ = 0.01; // previously tried 0.08;
    double wrist_ph_P_ = 1000.0;
    double wrist_ph_D_ = 10.0;

    std::array<PdController, N_rj_> robot_joint_pd_controllers_ = {
        PdController(elbow_P_, elbow_D_),
        PdController(forearm_P_, forearm_D_),
        PdController(prismatic_P_, prismatic_D_),
        PdController(prismatic_P_, prismatic_D_),
        PdController(prismatic_P_, prismatic_D_)
    };

    std::array<PdController, N_aj_> anatomical_joint_pd_controllers_ = {
        PdController(elbow_P_, elbow_D_),
        PdController(forearm_P_, forearm_D_),
        PdController(wrist_fe_P_, wrist_fe_D_),
        PdController(wrist_ru_P_, wrist_ru_D_),
        PdController(wrist_ph_P_, wrist_ph_D_)
    };     


private:


    // rps position control
    int rps_control_mode_ = 0; /// 0 = robot joint space (parallel), 1 = anatomical joint space (serial) with platform height backdrivable, 2 = anatomical joint space (serial) with all joints active
    bool rps_backdrive_ = false; /// true = rps is backdrivable, false = rps is active
    double rps_init_err_tol_ = 0.01; /// [m]
    std::vector<double> rps_par_goal_err_tol_ = std::vector<double>(N_qs_, 0.003);
    std::vector<double> rps_ser_goal_err_tol_ = { 2.0 * DEG2RAD, 2.0 * DEG2RAD, 0.005 };
    const std::vector<double> rps_init_pos_ = { 0.12, 0.12, 0.12 };
    const std::vector<double> rps_par_joint_speed_ = { 0.015, 0.015, 0.015 }; /// [m/s]
    const std::vector<double> rps_ser_joint_speed_ = { 0.125, 0.125, 0.015 }; /// [rad/s] and [m/s]


    // full robot position control
    bool elbow_backdrive_ = false; /// true = elbow is backdrivable, false = elbow is active
    bool forearm_backdrive_ = false; /// true = forearm is backdrivable, false = forearm is active
    std::vector<double> anat_goal_err_tol_ = { 2.0 * DEG2RAD, 3.0 * DEG2RAD, 5.0 * DEG2RAD, 5.0 * DEG2RAD, 0.01 };
    std::vector<double> anat_neutral_err_tol_ = { 1.0 * DEG2RAD, 2.0 * DEG2RAD, 3.0 * DEG2RAD, 3.0 * DEG2RAD, 0.01 };
    const std::vector<double> robot_joint_speed_ = { 0.25, 0.25, 0.015, 0.015, 0.015 };
    //const std::vector<double> anat_joint_speed_ = { 0.25, 0.75, 0.25, 0.25, 0.015 }; // speeds used for teleop
    const std::vector<double> anat_joint_speed_ = { 0.25, 0.35, 0.15, 0.15, 0.015 }; /// [rad/s] and [m/s] constant speed at which anatomical joint reference trajectories are interpolated

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
    Eigen::VectorXd tau_par_rob_ = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd tau_ser_rob_ = Eigen::VectorXd::Zero(N_qs_);
    Eigen::MatrixXd rho_fk_ = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::MatrixXd jac_fk_ = Eigen::MatrixXd::Zero(N_qs_, N_qs_);

            

    // kinematics solver setup variables
    const uint32 max_it_ = 10;
    const double tol_ = 1e-12;
    const std::vector<uint8> select_q_par_ = { 3, 4, 5 };
    const std::vector<uint8> select_q_ser_ = { 6, 7, 9 };

    double spec_norm_prev_ = 0; // debugging
    Eigen::VectorXd q_par_prev_ = Eigen::VectorXd::Zero(N_qs_); // debugging        

    // solving for static equilibrium joint torques           
    void solve_static_rps_torques(std::vector<uint8> select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s) const;
    void solve_static_rps_torques(std::vector<uint8> select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s, Eigen::VectorXd& tau_p) const;


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
    void solve_rps_kinematics(std::vector<uint8> select_q, const Eigen::VectorXd& qs, Eigen::VectorXd& qp, Eigen::MatrixXd& rho, Eigen::MatrixXd& rho_s, uint32 max_it, double tol) const;
    void generate_rho(std::vector<uint8> select_q, const Eigen::VectorXd& qp, Eigen::MatrixXd& rho) const;
    void psi_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qs, const Eigen::VectorXd& qp, Eigen::VectorXd& phi, Eigen::VectorXd& psi) const;
    void psi_d_qp_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp, Eigen::MatrixXd& psi_d_qp) const;
    void phi_update(const Eigen::VectorXd& qp, Eigen::VectorXd& phi) const;
    void phi_d_qp_update(const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp) const;
    std::vector<uint8> select_q_invert(std::vector<uint8> select_q) const;

    // PRIVATE UTILITY FUNCTIONS
    bool check_goal_pos(std::vector<double> goal_pos, std::vector<double> current_pos, std::vector<char> check_joint, std::vector<double> error_tol, bool print_output = false) const;


};

} // namespace mel

#endif // MEL_MAHIEXOII_HPP