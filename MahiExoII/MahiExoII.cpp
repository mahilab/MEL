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
    rho_sub_(Eigen::MatrixXd::Zero(3,3)),
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

    // run forward kinematics solver to update qp_ (q prime), which contains all 12 RPS positions
    forward_kinematics(10, 0.0000000001);

    // get positions from first two anatomical joints, which have encoders
    anatomical_joint_positions_[0] = joints_[0]->get_position();
    anatomical_joint_positions_[1] = joints_[1]->get_position();

    // get positions for two wrist anatomical joints from forward kinematics solver
    anatomical_joint_positions_[2] = qp_(6);
    anatomical_joint_positions_[3] = qp_(7);

    // run forward kinematics solver to update qp_dot_ (q prime time derivative), which contains all 12 RPS velocities
    forward_kinematics_velocity();

    // get velocities from first two anatomical joints, which have encoders
    anatomical_joint_velocities_[0] = joints_[0]->get_velocity();
    anatomical_joint_velocities_[1] = joints_[1]->get_velocity();

    // get velocities for two wrist anatomical joints from forward kinematics solver
    anatomical_joint_velocities_[2] = qp_dot_(6);
    anatomical_joint_velocities_[3] = qp_dot_(7);

}
    
double MahiExoII::get_anatomical_joint_position(int index) {
    return anatomical_joint_positions_[index];
}

double MahiExoII::get_anatomical_joint_velocity(int index) {
    return anatomical_joint_velocities_[index];
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

	// set torques for first two anatomical joints, which have actuators
	joints_[0]->set_torque(new_torques[0]);
	joints_[1]->set_torque(new_torques[1]);

	// set torques for two wrist anatomical joints
	Eigen::VectorXd par_torques = Eigen::VectorXd::Zero(3);
	Eigen::VectorXd ser_torques = Eigen::VectorXd::Zero(3);
	ser_torques(0) = new_torques[2];
	ser_torques(1) = new_torques[3];
	par_torques = rho_sub_.transpose()*ser_torques;
	joints_[2]->set_torque(par_torques(0));
	joints_[3]->set_torque(par_torques(1));
	joints_[4]->set_torque(par_torques(2));

}


void MahiExoII::forward_kinematics(mel::uint32 max_it, double tol) {
        
    // update q_par_ (q parallel) with the three prismatic link positions
    q_par_ << joints_[2]->get_position(), joints_[3]->get_position(), joints_[4]->get_position();

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
        psi_func(); // calculate 9 constraints and tracking error on specified q_par
        psi_d_qp_func(); // derivative of psi w.r.t. qp, giving a 12x12 matrix      
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
    

}


void MahiExoII::forward_kinematics_velocity() {

    // update qs_dot_ (q star time derivative) with the three prismatic link velocities
    q_par_dot_ << joints_[2]->get_velocity(), joints_[3]->get_velocity(), joints_[4]->get_velocity();

    rho_ = psi_d_qp_.fullPivLu().solve(alpha_mat_);
    rho_sub_.row(0) = rho_.row(6);
    rho_sub_.row(1) = rho_.row(7);
    rho_sub_.row(2) = rho_.row(11);
    qp_dot_ = rho_*q_par_dot_;

}


void MahiExoII::psi_func() {

    psi_ << qp_[3] * sin(qp_[0]) - qp_[9] - r_*cos(alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) - r_*sin(alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])),
        R_*cos(alpha5_) - qp_[10] - a56_*sin(alpha5_) - qp_[3] * cos(alpha5_)*cos(qp_[0]) - r_*cos(alpha13_)*cos(qp_[7])*cos(qp_[8]) + r_*cos(qp_[7])*sin(alpha13_)*sin(qp_[8]),
        a56_*cos(alpha5_) - qp_[11] + R_*sin(alpha5_) - qp_[3] * sin(alpha5_)*cos(qp_[0]) - r_*cos(alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) - r_*sin(alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])),
        qp_[4] * sin(qp_[1]) - qp_[9] - r_*cos(alpha13_ - (2 * mel::PI) / 3)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) - r_*sin(alpha13_ - (2 * mel::PI) / 3)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])),
        R_*cos(alpha5_ - (2 * mel::PI) / 3) - qp_[10] - a56_*sin(alpha5_ - (2 * mel::PI) / 3) - qp_[4] * cos(alpha5_ - (2 * mel::PI) / 3)*cos(qp_[1]) - r_*cos(qp_[7])*cos(qp_[8])*cos(alpha13_ - (2 * mel::PI) / 3) + r_*cos(qp_[7])*sin(qp_[8])*sin(alpha13_ - (2 * mel::PI) / 3),
        a56_*cos(alpha5_ - (2 * mel::PI) / 3) - qp_[11] + R_*sin(alpha5_ - (2 * mel::PI) / 3) - qp_[4] * cos(qp_[1])*sin(alpha5_ - (2 * mel::PI) / 3) - r_*cos(alpha13_ - (2 * mel::PI) / 3)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) - r_*sin(alpha13_ - (2 * mel::PI) / 3)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])),
        qp_[5] * sin(qp_[2]) - qp_[9] - r_*cos((2 * mel::PI) / 3 + alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) - r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])),
        R_*cos((2 * mel::PI) / 3 + alpha5_) - qp_[10] - a56_*sin((2 * mel::PI) / 3 + alpha5_) - qp_[5] * cos((2 * mel::PI) / 3 + alpha5_)*cos(qp_[2]) - r_*cos(qp_[7])*cos(qp_[8])*cos((2 * mel::PI) / 3 + alpha13_) + r_*cos(qp_[7])*sin(qp_[8])*sin((2 * mel::PI) / 3 + alpha13_),
        a56_*cos((2 * mel::PI) / 3 + alpha5_) - qp_[11] + R_*sin((2 * mel::PI) / 3 + alpha5_) - qp_[5] * cos(qp_[2])*sin((2 * mel::PI) / 3 + alpha5_) - r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) - r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])),
        qp_[3]-q_par_[0],
        qp_[4]-q_par_[1],
        qp_[5]-q_par_[2];

}

void MahiExoII::psi_d_qp_func() {

    psi_d_qp_ << qp_[3] * cos(qp_[0]), 0, 0, sin(qp_[0]), 0, 0, -r_*cos(alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) - r_*sin(alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])), r_*cos(alpha13_ + qp_[8])*cos(qp_[6])*cos(qp_[7]), r_*sin(alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) - r_*cos(alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])), -1, 0, 0,
        qp_[3] * cos(alpha5_)*sin(qp_[0]), 0, 0, -cos(alpha5_)*cos(qp_[0]), 0, 0, 0, r_*cos(alpha13_ + qp_[8])*sin(qp_[7]), r_*sin(alpha13_ + qp_[8])*cos(qp_[7]), 0, -1, 0,
        qp_[3] * sin(alpha5_)*sin(qp_[0]), 0, 0, -sin(alpha5_)*cos(qp_[0]), 0, 0, r_*cos(alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) + r_*sin(alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])), -r_*cos(alpha13_ + qp_[8])*cos(qp_[7])*sin(qp_[6]), r_*sin(alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) - r_*cos(alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])), 0, 0, -1,
        0, qp_[4] * cos(qp_[1]), 0, 0, sin(qp_[1]), 0, r_*cos(mel::PI / 3 + alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) + r_*sin(mel::PI / 3 + alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])), -r_*cos(qp_[6])*cos(qp_[7])*(cos(alpha13_ + qp_[8]) / 2 - (sqrt(3)*sin(alpha13_ + qp_[8])) / 2), r_*cos(mel::PI / 3 + alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])) - r_*sin(mel::PI / 3 + alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])), -1, 0, 0,
        0, -qp_[4] * cos(mel::PI / 3 + alpha5_)*sin(qp_[1]), 0, 0, cos(mel::PI / 3 + alpha5_)*cos(qp_[1]), 0, 0, -r_*sin(qp_[7])*(cos(alpha13_ + qp_[8]) / 2 - (sqrt(3)*sin(alpha13_ + qp_[8])) / 2), -r_*cos(qp_[7])*(sin(alpha13_ + qp_[8]) / 2 + (sqrt(3)*cos(alpha13_ + qp_[8])) / 2), 0, -1, 0,
        0, -qp_[4] * sin(mel::PI / 3 + alpha5_)*sin(qp_[1]), 0, 0, cos(qp_[1])*sin(mel::PI / 3 + alpha5_), 0, -r_*cos(mel::PI / 3 + alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) - r_*sin(mel::PI / 3 + alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])), r_*cos(qp_[7])*sin(qp_[6])*(cos(alpha13_ + qp_[8]) / 2 - (sqrt(3)*sin(alpha13_ + qp_[8])) / 2), r_*cos(mel::PI / 3 + alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])) - r_*sin(mel::PI / 3 + alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])), 0, 0, -1,
        0, 0, qp_[5] * cos(qp_[2]), 0, 0, sin(qp_[2]), -r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) - r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])), -r_*cos(qp_[6])*cos(qp_[7])*(cos(alpha13_ + qp_[8]) / 2 + (sqrt(3)*sin(alpha13_ + qp_[8])) / 2), r_*sin((2 * mel::PI) / 3 + alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) - r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])), -1, 0, 0,
        0, 0, qp_[5] * cos((2 * mel::PI) / 3 + alpha5_)*sin(qp_[2]), 0, 0, cos(qp_[2])*sin(mel::PI / 6 + alpha5_), 0, -r_*sin(qp_[7])*(cos(alpha13_ + qp_[8]) / 2 + (sqrt(3)*sin(alpha13_ + qp_[8])) / 2), -r_*cos(qp_[7])*(sin(alpha13_ + qp_[8]) / 2 - (sqrt(3)*cos(alpha13_ + qp_[8])) / 2), 0, -1, 0,
        0, 0, qp_[5] * sin((2 * mel::PI) / 3 + alpha5_)*sin(qp_[2]), 0, 0, -cos(qp_[2])*sin((2 * mel::PI) / 3 + alpha5_), r_*cos((2 * mel::PI) / 3 + alpha13_)*(sin(qp_[6])*sin(qp_[8]) - cos(qp_[6])*cos(qp_[8])*sin(qp_[7])) + r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[8])*sin(qp_[6]) + cos(qp_[6])*sin(qp_[7])*sin(qp_[8])), r_*cos(qp_[7])*sin(qp_[6])*(cos(alpha13_ + qp_[8]) / 2 + (sqrt(3)*sin(alpha13_ + qp_[8])) / 2), r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[6])*sin(qp_[8]) + cos(qp_[8])*sin(qp_[6])*sin(qp_[7])) - r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp_[6])*cos(qp_[8]) - sin(qp_[6])*sin(qp_[7])*sin(qp_[8])), 0, 0, -1,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0;
}