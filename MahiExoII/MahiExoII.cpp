#include "MahiExoII.h"



MahiExoII::MahiExoII(Config configuration, Params parameters) :
    Exo("mahi_exo_ii"),
    config_(configuration),
    params_(parameters),
    qp_(Eigen::VectorXd::Zero(12)),
    qp_0_(Eigen::VectorXd::Zero(12)),
    q_par_(Eigen::VectorXd::Zero(3)),
    q_ser_(Eigen::VectorXd::Zero(3)),
    qp_dot_(Eigen::VectorXd::Zero(12)),
    q_par_dot_(Eigen::VectorXd::Zero(3)),
    q_ser_dot_(Eigen::VectorXd::Zero(3)),
    A_(Eigen::MatrixXd::Zero(3, 12)),
    psi_(Eigen::VectorXd::Zero(12)),
    psi_d_qp_(Eigen::MatrixXd::Zero(12, 12)),
    rho_(Eigen::MatrixXd::Zero(12,3)),
    rho_sub_(Eigen::MatrixXd::Zero(3,3)),
    selector_mat_(Eigen::MatrixXd::Zero(12,3))
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

        anatomical_joint_positions_.push_back(0);
        anatomical_joint_velocities_.push_back(0);
        anatomical_joint_torques_.push_back(0);
    }
    qp_0_ << mel::PI / 4, mel::PI / 4, mel::PI / 4, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0.0923, 0, 0;
    selector_mat_.bottomRows(3) = Eigen::MatrixXd::Identity(3, 3);

    

}


void MahiExoII::anatomical_joint_set_points(mel::double_vec& set_points) {

    mel::double_vec torques = { 0,0,0,0,0 };

    torques[0] = mel::pd_controller(35, 0.25, set_points[0], anatomical_joint_positions_[0], 0, anatomical_joint_velocities_[0]);
    torques[1] = mel::pd_controller( 7, 0.06, set_points[1], anatomical_joint_positions_[1], 0, anatomical_joint_velocities_[1]);

    Eigen::VectorXd q_ser_ref = Eigen::VectorXd::Zero(3);
    Eigen::VectorXd q_par_ref = Eigen::VectorXd::Zero(3);
    q_ser_ref << set_points[2], set_points[3], 0.1;
    inverse_kinematics(q_ser_ref, q_par_ref);

    torques[2] = mel::pd_controller(10, 0.01, q_par_ref[0], joints_[2]->get_position(), 0, joints_[2]->get_velocity());
    torques[3] = mel::pd_controller(10, 0.01, q_par_ref[1], joints_[3]->get_position(), 0, joints_[3]->get_velocity());
    torques[4] = mel::pd_controller(10, 0.01, q_par_ref[2], joints_[4]->get_position(), 0, joints_[4]->get_velocity());

    joints_[0]->set_torque(torques[0]);
    joints_[1]->set_torque(torques[1]);
    joints_[2]->set_torque(torques[2]);
    joints_[3]->set_torque(torques[3]);
    joints_[4]->set_torque(torques[4]);

}


void MahiExoII::update_kinematics() {

    // update q_par_ (q parallel) with the three prismatic link positions
    q_par_ << joints_[2]->get_position(), joints_[3]->get_position(), joints_[4]->get_position();
   
    // run forward kinematics solver to update qp_ (q prime), which contains all 12 RPS positions
    forward_kinematics(q_par_, q_ser_, qp_);

    // get positions from first two anatomical joints, which have encoders
    anatomical_joint_positions_[0] = joints_[0]->get_position(); // elbow flexion/extension
    anatomical_joint_positions_[1] = joints_[1]->get_position(); // forearm pronation/supination

    // get positions from forward kinematics solver for three wrist anatomical joints 
    anatomical_joint_positions_[2] = qp_(6); // wrist flexion/extension
    anatomical_joint_positions_[3] = qp_(7); // wrist radial/ulnar deviation
    anatomical_joint_positions_[4] = qp_(9); // arm translation

    // run forward kinematics solver to update qp_dot_ (q prime time derivative), which contains all 12 RPS velocities
    // update qs_dot_ (q star time derivative) with the three prismatic link velocities
    q_par_dot_ << joints_[2]->get_velocity(), joints_[3]->get_velocity(), joints_[4]->get_velocity();
    forward_kinematics_velocity(q_par_dot_, q_ser_dot_, qp_dot_);

    // get velocities from first two anatomical joints, which have encoders
    anatomical_joint_velocities_[0] = joints_[0]->get_velocity(); // elbow flexion/extension
    anatomical_joint_velocities_[1] = joints_[1]->get_velocity(); // forearm pronation/supination

    // get velocities from forward kinematics solver for three wrist anatomical joints 
    anatomical_joint_velocities_[2] = qp_dot_(6); // wrist flexion/extension
    anatomical_joint_velocities_[3] = qp_dot_(7); // wrist radial/ulnar deviation
    anatomical_joint_velocities_[4] = qp_dot_(9); // arm translation

}



void MahiExoII::set_anatomical_joint_torques(mel::double_vec new_torques) {

	// set torques for first two anatomical joints, which have actuators
	joints_[0]->set_torque(new_torques[0]);
	joints_[1]->set_torque(new_torques[1]);

    mel::uint8_vec select_q = { 3,4,5 }; // indexing of qp                                  
    // build selection matrix
    A_ = Eigen::MatrixXd::Zero(3, 12);
    A_(0, select_q[0]) = 1;
    A_(1, select_q[1]) = 1;
    A_(2, select_q[2]) = 1;
    psi_d_qp_update(qp_);
    Eigen::MatrixXd rho = psi_d_qp_.fullPivLu().solve(selector_mat_);
    Eigen::MatrixXd rho_sub = Eigen::MatrixXd::Zero(3, 3);
    rho_sub.row(0) = rho.row(6);
    rho_sub.row(1) = rho.row(7);
    rho_sub.row(2) = rho.row(9);

	// set torques for two wrist anatomical joints
	Eigen::VectorXd par_torques = Eigen::VectorXd::Zero(3);
	Eigen::VectorXd ser_torques = Eigen::VectorXd::Zero(3);
	ser_torques(0) = new_torques[2];
	ser_torques(1) = new_torques[3];
	par_torques = rho_sub.transpose()*ser_torques;
	joints_[2]->set_torque(par_torques(0));
	joints_[3]->set_torque(par_torques(1));
	joints_[4]->set_torque(par_torques(2));

}

bool reached_anatomical_joint_position_target(double target_position) {



}



void MahiExoII::forward_kinematics(Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out) {

    mel::uint8_vec select_q = { 3,4,5 }; // indexing of qp_

    Eigen::VectorXd qp = solve_kinematics(select_q, q_par_in, max_it_, tol_);

    q_ser_out << qp(6), qp(7), qp(9);
   
}

void MahiExoII::forward_kinematics(Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out) {

    mel::uint8_vec select_q = { 3,4,5 }; // indexing of qp_

    qp_out = solve_kinematics(select_q, q_par_in, max_it_, tol_);

    q_ser_out << qp_out(6), qp_out(7), qp_out(9);

}

void MahiExoII::inverse_kinematics(Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out) {

    mel::uint8_vec select_q = { 6,7,9 }; // indexing of qp_

    Eigen::VectorXd qp = solve_kinematics(select_q, q_ser_in, max_it_, tol_);

    q_par_out << qp(3), qp(4), qp(5);

}

void MahiExoII::inverse_kinematics(Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out) {

    mel::uint8_vec select_q = { 6,7,9 }; // indexing of qp_

    qp_out = solve_kinematics(select_q, q_ser_in, max_it_, tol_);

    q_par_out << qp_out(3), qp_out(4), qp_out(5);

}



void MahiExoII::forward_kinematics_velocity(Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) {

    mel::uint8_vec select_q = { 3,4,5 }; // indexing of qp_

    // build selection matrix
    A_ = Eigen::MatrixXd::Zero(3, 12);
    A_(0, select_q[0]) = 1;
    A_(1, select_q[1]) = 1;
    A_(2, select_q[2]) = 1;

    psi_d_qp_update(qp_);
    Eigen::MatrixXd rho = psi_d_qp_.fullPivLu().solve(selector_mat_);
    //rho_sub_.row(0) = rho_.row(6);
    //rho_sub_.row(1) = rho_.row(7);
    //rho_sub_.row(2) = rho_.row(9);

    Eigen::VectorXd qp_dot = rho*q_par_dot_in;

    q_ser_dot_out << qp_dot(6), qp_dot(7), qp_dot(9);

}


void MahiExoII::forward_kinematics_velocity(Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out) {

    mel::uint8_vec select_q = { 3,4,5 }; // indexing of qp_

                                         // build selection matrix
    A_ = Eigen::MatrixXd::Zero(3, 12);
    A_(0, select_q[0]) = 1;
    A_(1, select_q[1]) = 1;
    A_(2, select_q[2]) = 1;

    psi_d_qp_update(qp_);
    Eigen::MatrixXd rho = psi_d_qp_.fullPivLu().solve(selector_mat_);

    qp_dot_out = rho*q_par_dot_in;

    q_ser_dot_out << qp_dot_out(6), qp_dot_out(7), qp_dot_out(9);

}


Eigen::VectorXd MahiExoII::solve_kinematics(mel::uint8_vec select_q, Eigen::VectorXd& qs, mel::uint32 max_it, double tol) {
    
    // build selection matrix
    A_ = Eigen::MatrixXd::Zero(3, 12);
    A_(0, select_q[0]) = 1;
    A_(1, select_q[1]) = 1;
    A_(2, select_q[2]) = 1;

    // declare and initialize variable containing solution
    Eigen::VectorXd qp;
    qp = qp_0_;

    // declare and initialize variables for keeping track of error
    double err = 2*tol;
    double a = 0;
    double b = 0;
    double c = 0;
    bool first_non_zero = true;
    
    // run no more than max_it iterations of updating the solution for qp_
    // exit loop once the error is below the input tolerance
    mel::uint32 it = 0;
    while (it < max_it && err > tol) {
        psi_update(qs, qp); // calculate 9 constraints and tracking error on specified qs_
        psi_d_qp_update(qp); // derivative of psi w.r.t. qp, giving a 12x12 matrix      
        qp -= psi_d_qp_.fullPivLu().solve(psi_);

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
    
    return qp;
    
}


void MahiExoII::psi_update(Eigen::VectorXd& qs, Eigen::VectorXd& qp) {
    
    psi_.head(9) = phi_func(qp);
    psi_.tail(3) = a_func(qp) - qs;

}


Eigen::VectorXd MahiExoII::a_func(Eigen::VectorXd& qp) {
    
    return A_*qp;

}


void MahiExoII::psi_d_qp_update(Eigen::VectorXd& qp) {

    psi_d_qp_.block<9, 12>(0, 0) = phi_d_qp_func(qp);
    psi_d_qp_.block<3, 12>(9, 0) = A_;

}

Eigen::VectorXd MahiExoII::phi_func(Eigen::VectorXd& qp) {
    
    Eigen::VectorXd phi = Eigen::VectorXd::Zero(9);

    phi << qp[3] * sin(qp[0]) - qp[9] - r_*cos(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R_*cos(alpha5_) - qp[10] - a56_*sin(alpha5_) - qp[3] * cos(alpha5_)*cos(qp[0]) - r_*cos(alpha13_)*cos(qp[7])*cos(qp[8]) + r_*cos(qp[7])*sin(alpha13_)*sin(qp[8]),
        a56_*cos(alpha5_) - qp[11] + R_*sin(alpha5_) - qp[3] * sin(alpha5_)*cos(qp[0]) - r_*cos(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[4] * sin(qp[1]) - qp[9] - r_*cos(alpha13_ - (2 * mel::PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R_*cos(alpha5_ - (2 * mel::PI) / 3) - qp[10] - a56_*sin(alpha5_ - (2 * mel::PI) / 3) - qp[4] * cos(alpha5_ - (2 * mel::PI) / 3)*cos(qp[1]) - r_*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * mel::PI) / 3) + r_*cos(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * mel::PI) / 3),
        a56_*cos(alpha5_ - (2 * mel::PI) / 3) - qp[11] + R_*sin(alpha5_ - (2 * mel::PI) / 3) - qp[4] * cos(qp[1])*sin(alpha5_ - (2 * mel::PI) / 3) - r_*cos(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[5] * sin(qp[2]) - qp[9] - r_*cos((2 * mel::PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R_*cos((2 * mel::PI) / 3 + alpha5_) - qp[10] - a56_*sin((2 * mel::PI) / 3 + alpha5_) - qp[5] * cos((2 * mel::PI) / 3 + alpha5_)*cos(qp[2]) - r_*cos(qp[7])*cos(qp[8])*cos((2 * mel::PI) / 3 + alpha13_) + r_*cos(qp[7])*sin(qp[8])*sin((2 * mel::PI) / 3 + alpha13_),
        a56_*cos((2 * mel::PI) / 3 + alpha5_) - qp[11] + R_*sin((2 * mel::PI) / 3 + alpha5_) - qp[5] * cos(qp[2])*sin((2 * mel::PI) / 3 + alpha5_) - r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8]));
    
    return phi;

}

Eigen::MatrixXd MahiExoII::phi_d_qp_func(Eigen::VectorXd& qp) {

    Eigen::MatrixXd phi_d_qp = Eigen::MatrixXd::Zero(9, 12);

    phi_d_qp << qp[3] * cos(qp[0]), 0, 0, sin(qp[0]), 0, 0, -r_*cos(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(alpha13_)*cos(qp[7])*cos(qp[8]) - r_*cos(qp[6])*cos(qp[7])*sin(alpha13_)*sin(qp[8]), r_*sin(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        qp[3] * cos(alpha5_)*sin(qp[0]), 0, 0, -cos(alpha5_)*cos(qp[0]), 0, 0, 0, r_*cos(alpha13_)*cos(qp[8])*sin(qp[7]) - r_*sin(alpha13_)*sin(qp[7])*sin(qp[8]), r_*cos(alpha13_)*cos(qp[7])*sin(qp[8]) + r_*cos(qp[7])*cos(qp[8])*sin(alpha13_), 0, -1, 0,
        qp[3] * sin(alpha5_)*sin(qp[0]), 0, 0, -sin(alpha5_)*cos(qp[0]), 0, 0, r_*cos(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(alpha13_)*sin(qp[8]) - r_*cos(alpha13_)*cos(qp[7])*cos(qp[8])*sin(qp[6]), r_*sin(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
        0, qp[4] * cos(qp[1]), 0, 0, sin(qp[1]), 0, -r_*cos(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * mel::PI) / 3) - r_*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * mel::PI) / 3), r_*sin(alpha13_ - (2 * mel::PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        0, qp[4] * cos(alpha5_ - (2 * mel::PI) / 3)*sin(qp[1]), 0, 0, -cos(alpha5_ - (2 * mel::PI) / 3)*cos(qp[1]), 0, 0, r_*cos(qp[8])*cos(alpha13_ - (2 * mel::PI) / 3)*sin(qp[7]) - r_*sin(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * mel::PI) / 3), r_*cos(qp[7])*cos(qp[8])*sin(alpha13_ - (2 * mel::PI) / 3) + r_*cos(qp[7])*cos(alpha13_ - (2 * mel::PI) / 3)*sin(qp[8]), 0, -1, 0,
        0, qp[4] * sin(alpha5_ - (2 * mel::PI) / 3)*sin(qp[1]), 0, 0, -cos(qp[1])*sin(alpha5_ - (2 * mel::PI) / 3), 0, r_*cos(alpha13_ - (2 * mel::PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin(alpha13_ - (2 * mel::PI) / 3) - r_*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * mel::PI) / 3)*sin(qp[6]), r_*sin(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos(alpha13_ - (2 * mel::PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
        0, 0, qp[5] * cos(qp[2]), 0, 0, sin(qp[2]), -r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos((2 * mel::PI) / 3 + alpha13_) - r_*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin((2 * mel::PI) / 3 + alpha13_), r_*sin((2 * mel::PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        0, 0, qp[5] * cos((2 * mel::PI) / 3 + alpha5_)*sin(qp[2]), 0, 0, -cos((2 * mel::PI) / 3 + alpha5_)*cos(qp[2]), 0, r_*cos(qp[8])*cos((2 * mel::PI) / 3 + alpha13_)*sin(qp[7]) - r_*sin(qp[7])*sin(qp[8])*sin((2 * mel::PI) / 3 + alpha13_), r_*cos(qp[7])*cos(qp[8])*sin((2 * mel::PI) / 3 + alpha13_) + r_*cos(qp[7])*cos((2 * mel::PI) / 3 + alpha13_)*sin(qp[8]), 0, -1, 0,
        0, 0, qp[5] * sin((2 * mel::PI) / 3 + alpha5_)*sin(qp[2]), 0, 0, -cos(qp[2])*sin((2 * mel::PI) / 3 + alpha5_), r_*cos((2 * mel::PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin((2 * mel::PI) / 3 + alpha13_) - r_*cos(qp[7])*cos(qp[8])*cos((2 * mel::PI) / 3 + alpha13_)*sin(qp[6]), r_*sin((2 * mel::PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos((2 * mel::PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1;

    return phi_d_qp;

}
