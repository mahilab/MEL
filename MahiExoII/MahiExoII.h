#pragma once
#include "Exo.h"
#include "util.h"
#include <Eigen\Dense>
#include <Eigen\LU>
#include <array>


class MahiExoII : public mel::Exo {

public:

    struct Config {
        std::array<mel::Daq::Do, 5>      enable_;    // digital output channels that enable motors
        std::array<mel::Daq::Ao, 5>      command_;   // analog output channels that command motors
        std::array<mel::Daq::Encoder, 5> encoder_;   // encoder channels that measure motor positions
        std::array<mel::Daq::EncRate, 5> encrate_;   // encoder channels that measure motor velocities
        std::array<double, 5>            amp_gains_; // motor aplifier gains 
    };

    struct Params {
        std::array<double, 5>      eta_ = { 0.42 / 4.5, 0.0662864, mel::INCH2METER*0.23, mel::INCH2METER*0.23, mel::INCH2METER*0.23 }; // transmission ratios    [inch / inch]
        std::array<mel::uint32, 5> encoder_res_ = { 2048, 2048, 2048, 2048, 2048 };           // encoder resolutions    [counts / rev]
        std::array<double, 5>      kt_ = { 0.127, 0.0603, 0.175, 0.175, 0.175 };        // motor torque constants [N-m/A]
        std::array<double, 5>      current_limits_ = { 6.0, 3.17, 0.626, 0.626, 0.626 };          // motor current limits   [A]
    };

    MahiExoII(Config configuration, Params parameters = Params());

    const Config config_;
    const Params params_;

    void anatomical_joint_set_points(mel::double_vec& set_points);
    void update_kinematics();
    double get_anatomical_joint_position(int index);
    double get_anatomical_joint_velocity(int index);

    bool reached_anatomical_joint_position_target(double target_position);

    // inherited virtual functions from Exo class to be implemented
    mel::double_vec get_anatomical_joint_positions() override;
    mel::double_vec get_anatomical_joint_velocities() override;
    void set_anatomical_joint_torques(mel::double_vec new_torques) override;

private:

    
    // parameters
    const double R_ = 0.1044956;
    const double r_ = 0.05288174521;
    const double a56_ = 0.0268986 - 0.0272820;
    const double alpha5_ = 0.094516665054824;
    const double alpha13_ = mel::DEG2RAD * 5;
    

    // kinematics variables
    Eigen::VectorXd qp_;
    Eigen::VectorXd qp_0_;
    Eigen::VectorXd q_par_;
    Eigen::VectorXd q_ser_;
    Eigen::VectorXd qp_dot_;
    Eigen::VectorXd q_par_dot_;
    Eigen::VectorXd q_ser_dot_;
    Eigen::MatrixXd A_;
    Eigen::VectorXd psi_;
    Eigen::MatrixXd psi_d_qp_;
    Eigen::MatrixXd rho_;
    Eigen::MatrixXd rho_sub_;
    Eigen::MatrixXd selector_mat_;
    const mel::uint32 max_it_ = 10;
    const double tol_ = 1e-12;


    
    void forward_kinematics(Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out);
    void forward_kinematics(Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out);
    void inverse_kinematics(Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out);
    void inverse_kinematics(Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out);
    void forward_kinematics_velocity(Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out);
    void forward_kinematics_velocity(Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out);
    Eigen::VectorXd solve_kinematics(mel::uint8_vec select_q, Eigen::VectorXd& qs, mel::uint32 max_it, double tol);
    void psi_update(Eigen::VectorXd& qs, Eigen::VectorXd& qp);
    void psi_d_qp_update(Eigen::VectorXd& qp);
    Eigen::VectorXd phi_func(Eigen::VectorXd& qp);
    Eigen::VectorXd a_func(Eigen::VectorXd& qp);
    Eigen::MatrixXd phi_d_qp_func(Eigen::VectorXd& qp);
    
    
	
    
    
    
    
    
};
