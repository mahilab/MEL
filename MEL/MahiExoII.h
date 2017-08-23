#pragma once
#include "Exo.h"
#include "util.h"
#include <Eigen\Dense>
#include <Eigen\LU>
#include <array>
#include "Motor.h"
#include "Encoder.h"

class MahiExoII : public mel::Exo {

public:

    //-------------------------------------------------------------------------
    // CONFIGURATION / PARAMETERS STRUCTS
    //-------------------------------------------------------------------------

    struct Config {
        std::array<mel::Daq::Do, 5>      enable_;    ///< digital output channels that enable motors
        std::array<mel::Daq::Ao, 5>      command_;   ///< analog output channels that command motors
        std::array<mel::Daq::Encoder, 5> encoder_;   ///< encoder channels that measure motor positions
        std::array<mel::Daq::EncRate, 5> encrate_;   ///< encoder channels that measure motor velocities
    };

    struct Params {
        std::array<double, 5> kt_ = { 0.127, 0.0603, 0.175, 0.175, 0.175 }; ///< motor torque constants [N-m/A]
        std::array<double, 5> motor_continuous_current_limits_ = { 6.0, 3.17, 0.626, 0.626, 0.626 }; ///< motor continuous current limits [A]
        std::array<double, 5> motor_peak_current_limits_ = { 18.0, 18.0, 1.8, 1.8, 1.8 }; ///< motor peak current limits [A]
        std::array<double, 5> motor_i2t_times_ = { 2, 2, 2, 2, 2 }; ///< motor i^2*t times [s]
        std::array<double, 5> eta_ = { 0.42 / 4.5, 0.0662864, mel::INCH2METER*0.23, mel::INCH2METER*0.23, mel::INCH2METER*0.23 }; ///< transmission ratios [inch/inch] or [m]
        std::array<mel::uint32, 5> encoder_res_ = { 2048, 2048, 2048, 2048, 2048 }; ///< encoder resolutions [counts/rev]
        std::array<double, 5> pos_limits_min_ = { -91.5 * mel::DEG2RAD, -99 * mel::DEG2RAD, 0.050, 0.050, 0.050 }; ///< robot joint position limits in negative direction [rad] or [m]
        std::array<double, 5> pos_limits_max_ = {     3 * mel::DEG2RAD, 108 * mel::DEG2RAD, 0.133, 0.133, 0.133 }; ///< robot joint position limits in positive direction [rad] or [m]
        std::array<double, 5> vel_limits_ = { 250 * mel::DEG2RAD, 300 * mel::DEG2RAD, 0.4, 0.4, 0.4 }; ///< robot joint velocity limits [rad/s] or [m/s]
        std::array<double, 5> joint_torque_limits = { 10, 1, 20, 20, 20 }; ///< robot joint torque limits [Nm] or [N]
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
    
    //void anatomical_joint_set_points(mel::double_vec& set_points);
    void update_kinematics();

    // inherited virtual functions from Exo class to be implemented
    void set_anatomical_joint_torques(mel::double_vec new_torques) override;

    //-------------------------------------------------------------------------
    // PUBLIC VARIABLES
    //-------------------------------------------------------------------------

    const Config config_;
    const Params params_;

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

    //-------------------------------------------------------------------------
    // PRIVATE FUNCTIONS
    //-------------------------------------------------------------------------

    // kinematics functions
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
