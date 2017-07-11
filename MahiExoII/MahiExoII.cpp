#include "MahiExoII.h"



MahiExoII::MahiExoII(Config configuration, Params parameters) :
    Exo("mahi_exo_ii"),
    config_(configuration),
    params_(parameters),
    qp_(Eigen::VectorXd::Zero(12)),
    qp_dot_(Eigen::VectorXd::Zero(12)),
    qp_0_(Eigen::VectorXd::Zero(12)),
    q_par_(Eigen::VectorXd::Zero(3)),
    q_par_dot_(Eigen::VectorXd::Zero(3)),
    psi_(Eigen::VectorXd::Zero(12)),
    psi_d_qp_(Eigen::MatrixXd::Zero(12, 12)),
    rho_(Eigen::MatrixXd::Zero(12,3)),
    alpha_mat_(Eigen::MatrixXd::Zero(12,3)),
    X_mat_(Eigen::MatrixXd::Zero(12,12))
{

    for (int i = 0; i < 5; i++) {

        std::string num = std::to_string(i);

        mel::PositionSensor* encoder = new mel::Encoder("encoder_" + num,
            params_.encoder_res_[i] / (2 * mel::PI),
            config_.encoder_[i],
            config_.encrate_[i]);

        position_sensors_.push_back(encoder);

        mel::Actuator* motor = new mel::Motor("motor_" + num,
            params_.kt_[i],
            params_.current_limits_[i],
            config_.amp_gains_[i],
            config_.command_[i],
            config_.enable_[i],
            mel::Actuator::EnableMode::Low);

        actuators_.push_back(motor);

        mel::Joint* joint = new mel::Joint("joint_" + num,
            encoder,
            params_.eta_[i],
            motor,
            params_.eta_[i]);

        joints_.push_back(joint);
    }
    qp_0_ << mel::PI / 4, mel::PI / 4, mel::PI / 4, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0, 0, 0.1305;
    alpha_mat_.bottomRows(3) = Eigen::MatrixXd::Identity(3, 3);

}

void MahiExoII::update_kinematics() {

    // update q_par_ (q parallel) with the three prismatic link positions
    q_par_ << joints_[2]->get_position(), joints_[3]->get_position(), joints_[4]->get_position();

    // run forward kinematics solver to update qp_ (q prime), which contains all 12 RPS positions
    forward_kinematics(&qp_, q_par_, 10, 0.0000000001);

    // update qs_dot_ (q star time derivative) with the three prismatic link velocities
    q_par_dot_ << joints_[2]->get_velocity(), joints_[3]->get_velocity(), joints_[4]->get_velocity();

    // run forward kinematics solver to update qp_dot_ (q prime time derivative), which contains all 12 RPS velocities
    forward_kinematics_velocity(&qp_dot_, q_par_dot_);

}
    

mel::double_vec MahiExoII::get_anatomical_joint_positions() {

    // get positions from first two anatomical joints, which have encoders
    anatomical_joint_positions_[0] = joints_[0]->get_position();
    anatomical_joint_positions_[1] = joints_[1]->get_position();

    // get positions for two wrist anatomical joints from forward kinematics solver
    anatomical_joint_positions_[2] = qp_(6);
    anatomical_joint_positions_[3] = qp_(7);

    return anatomical_joint_positions_;
}

mel::double_vec MahiExoII::get_anatomical_joint_velocities() {

    // get velocities from first two anatomical joints, which have encoders
    anatomical_joint_velocities_[0] = joints_[0]->get_velocity();
    anatomical_joint_velocities_[1] = joints_[1]->get_velocity();

    // get velocities for two wrist anatomical joints from forward kinematics solver
    anatomical_joint_velocities_[2] = qp_dot_(6);
    anatomical_joint_velocities_[3] = qp_dot_(7);

    return anatomical_joint_velocities_;
}

void MahiExoII::set_anatomical_joint_torques(mel::double_vec new_torques) {


}


void MahiExoII::forward_kinematics(Eigen::VectorXd* qp, Eigen::VectorXd q_par, mel::uint32 max_it, double tol) {
        
    // declare and initialize variables for keeping track of error
    double err = 2*tol;
    double a = 0;
    double b = 0;
    double c = 0;
    bool first_non_zero = true;
    qp_ = qp_0_;

    // run no more than max_it iterations of updating the solution for qp_
    // exit loop once the error is below the input tolerance
    mel::uint32 it = 0;
    while (it < max_it && err > tol) {
        psi_func(&psi_, qp_, q_par, R_, r_, a56_, alpha5_, alpha13_); // actually psi bar, which evaluates 9 constraints and three desired values
        psi_d_qp_func(&psi_d_qp_, qp_, r_, alpha5_, alpha13_); // derivative of psi w.r.t. qp, giving a 12x12 matrix
        
        qp_ -= psi_d_qp_.fullPivLu().solve(psi_);

        // update the error (don't know why it's like this, but it seems to work)
        err = 0;
        c = 0;
        first_non_zero = true;
        for (auto j = 0; j != 12; ++j) {
            a = psi_[j];
            if (a != 0) {
                a = fabs(a);
                if (first_non_zero) {
                    c = a;
                    err = 1.0;
                    first_non_zero = false;
                }
                else if (c < a) {
                    b = c / a;
                    err = 1.0 + err * b * b;
                    c = a;
                }
                else {
                    b = a / c;
                    err += b * b;
                }
            }
        }
        err = c*sqrt(err);


        // while iterator
        it++;
    }
    *qp = qp_;
    

}


void MahiExoII::forward_kinematics_velocity(Eigen::VectorXd* qp_dot, Eigen::VectorXd q_par_dot) {

    rho_ = psi_d_qp_.fullPivLu().solve(alpha_mat_);
    *qp_dot = rho_*q_par_dot;
    
    //rho_ = psi_d_qp_.fullPivLu().solve(alpha_mat_*q_par_dot);
    //X_mat_ = psi_d_qp_.fullPivLu().solve(alpha_mat_*q_par_dot);
    //rho_ = X_mat_.block<9,3>(0,9);
}


void MahiExoII::psi_func(Eigen::VectorXd* psi, Eigen::VectorXd qp, Eigen::VectorXd q_par, double R, double r, double a56, double alpha5, double alpha13) {

    *psi << qp[3] * sin(qp[0]) - qp[9] - r*cos(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R*cos(alpha5) - qp[10] - a56*sin(alpha5) - qp[3] * cos(alpha5)*cos(qp[0]) - r*cos(alpha13)*cos(qp[7])*cos(qp[8]) + r*cos(qp[7])*sin(alpha13)*sin(qp[8]),
        a56*cos(alpha5) - qp[11] + R*sin(alpha5) - qp[3] * sin(alpha5)*cos(qp[0]) - r*cos(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[4] * sin(qp[1]) - qp[9] - r*cos(alpha13 - (2 * mel::PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin(alpha13 - (2 * mel::PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R*cos(alpha5 - (2 * mel::PI) / 3) - qp[10] - a56*sin(alpha5 - (2 * mel::PI) / 3) - qp[4] * cos(alpha5 - (2 * mel::PI) / 3)*cos(qp[1]) - r*cos(qp[7])*cos(qp[8])*cos(alpha13 - (2 * mel::PI) / 3) + r*cos(qp[7])*sin(qp[8])*sin(alpha13 - (2 * mel::PI) / 3),
        a56*cos(alpha5 - (2 * mel::PI) / 3) - qp[11] + R*sin(alpha5 - (2 * mel::PI) / 3) - qp[4] * cos(qp[1])*sin(alpha5 - (2 * mel::PI) / 3) - r*cos(alpha13 - (2 * mel::PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13 - (2 * mel::PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[5] * sin(qp[2]) - qp[9] - r*cos((2 * mel::PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin((2 * mel::PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R*cos((2 * mel::PI) / 3 + alpha5) - qp[10] - a56*sin((2 * mel::PI) / 3 + alpha5) - qp[5] * cos((2 * mel::PI) / 3 + alpha5)*cos(qp[2]) - r*cos(qp[7])*cos(qp[8])*cos((2 * mel::PI) / 3 + alpha13) + r*cos(qp[7])*sin(qp[8])*sin((2 * mel::PI) / 3 + alpha13),
        a56*cos((2 * mel::PI) / 3 + alpha5) - qp[11] + R*sin((2 * mel::PI) / 3 + alpha5) - qp[5] * cos(qp[2])*sin((2 * mel::PI) / 3 + alpha5) - r*cos((2 * mel::PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin((2 * mel::PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[3]-q_par[0],
        qp[4]-q_par[1],
        qp[5]-q_par[2];
}

void MahiExoII::psi_d_qp_func(Eigen::MatrixXd* psi_d_qp, Eigen::VectorXd qp, double r, double alpha5, double alpha13) {

    *psi_d_qp << qp[3] * cos(qp[0]), 0, 0, sin(qp[0]), 0, 0, -r*cos(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(alpha13 + qp[8])*cos(qp[6])*cos(qp[7]), r*sin(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        qp[3] * cos(alpha5)*sin(qp[0]), 0, 0, -cos(alpha5)*cos(qp[0]), 0, 0, 0, r*cos(alpha13 + qp[8])*sin(qp[7]), r*sin(alpha13 + qp[8])*cos(qp[7]), 0, -1, 0,
        qp[3] * sin(alpha5)*sin(qp[0]), 0, 0, -sin(alpha5)*cos(qp[0]), 0, 0, r*cos(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -r*cos(alpha13 + qp[8])*cos(qp[7])*sin(qp[6]), r*sin(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
        0, qp[4] * cos(qp[1]), 0, 0, sin(qp[1]), 0, r*cos(mel::PI / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) + r*sin(mel::PI / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), -r*cos(qp[6])*cos(qp[7])*(cos(alpha13 + qp[8]) / 2 - (sqrt(3)*sin(alpha13 + qp[8])) / 2), r*cos(mel::PI / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])) - r*sin(mel::PI / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])), -1, 0, 0,
        0, -qp[4] * cos(mel::PI / 3 + alpha5)*sin(qp[1]), 0, 0, cos(mel::PI / 3 + alpha5)*cos(qp[1]), 0, 0, -r*sin(qp[7])*(cos(alpha13 + qp[8]) / 2 - (sqrt(3)*sin(alpha13 + qp[8])) / 2), -r*cos(qp[7])*(sin(alpha13 + qp[8]) / 2 + (sqrt(3)*cos(alpha13 + qp[8])) / 2), 0, -1, 0,
        0, -qp[4] * sin(mel::PI / 3 + alpha5)*sin(qp[1]), 0, 0, cos(qp[1])*sin(mel::PI / 3 + alpha5), 0, -r*cos(mel::PI / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin(mel::PI / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*(cos(alpha13 + qp[8]) / 2 - (sqrt(3)*sin(alpha13 + qp[8])) / 2), r*cos(mel::PI / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])) - r*sin(mel::PI / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])), 0, 0, -1,
        0, 0, qp[5] * cos(qp[2]), 0, 0, sin(qp[2]), -r*cos((2 * mel::PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin((2 * mel::PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), -r*cos(qp[6])*cos(qp[7])*(cos(alpha13 + qp[8]) / 2 + (sqrt(3)*sin(alpha13 + qp[8])) / 2), r*sin((2 * mel::PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos((2 * mel::PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        0, 0, qp[5] * cos((2 * mel::PI) / 3 + alpha5)*sin(qp[2]), 0, 0, cos(qp[2])*sin(mel::PI / 6 + alpha5), 0, -r*sin(qp[7])*(cos(alpha13 + qp[8]) / 2 + (sqrt(3)*sin(alpha13 + qp[8])) / 2), -r*cos(qp[7])*(sin(alpha13 + qp[8]) / 2 - (sqrt(3)*cos(alpha13 + qp[8])) / 2), 0, -1, 0,
        0, 0, qp[5] * sin((2 * mel::PI) / 3 + alpha5)*sin(qp[2]), 0, 0, -cos(qp[2])*sin((2 * mel::PI) / 3 + alpha5), r*cos((2 * mel::PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin((2 * mel::PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*(cos(alpha13 + qp[8]) / 2 + (sqrt(3)*sin(alpha13 + qp[8])) / 2), r*sin((2 * mel::PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos((2 * mel::PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0;
}