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

    void update_kinematics();
    double get_anatomical_joint_position(int index);
    double get_anatomical_joint_velocity(int index);

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
    Eigen::VectorXd qp_0_;

    // state variables
    Eigen::VectorXd qp_;
    Eigen::VectorXd qp_prev_;
    Eigen::VectorXd qp_dot_;
    Eigen::VectorXd q_par_;
    Eigen::VectorXd q_par_dot_;
    Eigen::VectorXd psi_;
    Eigen::MatrixXd psi_d_qp_;
    Eigen::MatrixXd rho_;
    Eigen::MatrixXd rho_sub_;
    Eigen::MatrixXd alpha_mat_;
    Eigen::MatrixXd X_mat_;

    void forward_kinematics(mel::uint32 max_it, double tol);
    void forward_kinematics_velocity();
    void psi_func();
    void psi_d_qp_func();
    
};
