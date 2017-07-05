#include "MahiExoII.h"


MahiExoII::MahiExoII(Config configuration, Params parameters) :
    Exo("mahi_exo_ii"),
    config_(configuration),
    params_(parameters)
    //qp_(Eigen::VectorXd::Zero(12)),
    //qs_(Eigen::VectorXd::Zero(3)),
    //psi_(Eigen::VectorXd::Zero(12)),
    //psi_d_qp_(Eigen::MatrixXd::Zero(12, 12))
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
            mel::Actuator::EnableMode::High);

        actuators_.push_back(motor);

        mel::Joint* joint = new mel::Joint("joint_" + num,
            encoder,
            params_.eta_[i],
            motor,
            params_.eta_[i]);

        joints_.push_back(joint);

    }

}

    /*
    MahiExoII::MahiExoII(mel::Daq* daq_)
        //qp_(Eigen::VectorXd::Zero(12)),
        //qs_(Eigen::VectorXd::Zero(3)),
        //psi_(Eigen::VectorXd::Zero(12)),
        //psi_d_qp_(Eigen::MatrixXd::Zero(12, 12))
    {

        position_sensors_.push_back(new Encoder("Elbow",   2048 / (2 * PI), daq_->encoder_(0), daq_->encrate_(0) ));
        position_sensors_.push_back(new Encoder("Forearm", 2048 / (2 * PI), daq_->encoder_(1), daq_->encrate_(1) ));
        position_sensors_.push_back(new Encoder("Wrist 0", 2048 / (2 * PI), daq_->encoder_(2), daq_->encrate_(2) ));
        position_sensors_.push_back(new Encoder("Wrist 1", 2048 / (2 * PI), daq_->encoder_(3), daq_->encrate_(3) ));
        position_sensors_.push_back(new Encoder("Wrist 2", 2048 / (2 * PI), daq_->encoder_(4), daq_->encrate_(4) ));

        actuators_.push_back(new Motor( 0.127, 6.0, 1.8, daq_->ao_(0), daq_->do_(0) ));
        actuators_.push_back(new Motor( 0.0603, 3.17, 1.8, daq_->ao_(1), daq_->do_(1) ));
        actuators_.push_back(new Motor( 0.175, 0.626, 0.184, daq_->ao_(2), daq_->do_(2) ));
        actuators_.push_back(new Motor( 0.175, 0.626, 0.184, daq_->ao_(3), daq_->do_(3) ));
        actuators_.push_back(new Motor( 0.175, 0.626, 0.184, daq_->ao_(4), daq_->do_(4) ));

        joints_.push_back(new RevoluteRobotJoint(position_sensors_[0], 0.42 / 4.5, actuators_[0], 0.42 / 4.5)); // elbow flexion/extension
        joints_.push_back(new RevoluteRobotJoint(position_sensors_[1], 0.17 / 2.5, actuators_[1], 0.17 / 2.5)); // forearm pronation/supination
        joints_.push_back(new PrismaticRobotJoint(position_sensors_[2], INCH2METER*0.23, actuators_[2], INCH2METER*0.23)); // wrist prismatic l_1
        joints_.push_back(new PrismaticRobotJoint(position_sensors_[3], INCH2METER*0.23, actuators_[3], INCH2METER*0.23)); // wrist prismatic l_2
        joints_.push_back(new PrismaticRobotJoint(position_sensors_[4], INCH2METER*0.23, actuators_[4], INCH2METER*0.23)); // wrist prismatic l_3
        /*
        joints_.push_back(new RevoluteJoint()); // wrist revolute theta_1
        joints_.push_back(new RevoluteJoint()); // wrist revolute theta_2
        joints_.push_back(new RevoluteJoint()); // wrist revolute theta_3
        joints_.push_back(new PrismaticJoint()); // wrist platform position x_c
        joints_.push_back(new PrismaticJoint()); // wrist platform position y_c
        joints_.push_back(new PrismaticJoint()); // wrist platform position z_c
        joints_.push_back(new RevoluteJoint()); // wrist flexion/extension alpha
        joints_.push_back(new RevoluteJoint()); // wrist radial/ulnar deviation beta
        joints_.push_back(new RevoluteJoint()); // wrist platform roll gamma
        */
        //joint_positions_ = double_vec(joints_.size(), 0.0);

        //qp_ << -PI / 4, -PI / 4, -PI / 4, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0, 0, 0.1305;
                


    /*
    position_sensors_.push_back(new Encoder("Elbow",   2048 / (2 * PI), daq_->encoder_(0), daq_->encrate_(0) ));
    position_sensors_.push_back(new Encoder("Forearm", 2048 / (2 * PI), daq_->encoder_(1), daq_->encrate_(1) ));
    position_sensors_.push_back(new Encoder("Wrist 0", 2048 / (2 * PI), daq_->encoder_(2), daq_->encrate_(2) ));
    position_sensors_.push_back(new Encoder("Wrist 1", 2048 / (2 * PI), daq_->encoder_(3), daq_->encrate_(3) ));
    position_sensors_.push_back(new Encoder("Wrist 2", 2048 / (2 * PI), daq_->encoder_(4), daq_->encrate_(4) ));

    actuators_.push_back(new Motor( 0.127, 6.0, 1.8, daq_->ao_(0), daq_->do_(0) ));
    actuators_.push_back(new Motor( 0.0603, 3.17, 1.8, daq_->ao_(1), daq_->do_(1) ));
    actuators_.push_back(new Motor( 0.175, 0.626, 0.184, daq_->ao_(2), daq_->do_(2) ));
    actuators_.push_back(new Motor( 0.175, 0.626, 0.184, daq_->ao_(3), daq_->do_(3) ));
    actuators_.push_back(new Motor( 0.175, 0.626, 0.184, daq_->ao_(4), daq_->do_(4) ));

    robot_joints_.push_back(new RevoluteRobotJoint(position_sensors_[0], 0.42 / 4.5, actuators_[0], 0.42 / 4.5)); // elbow flexion/extension
    robot_joints_.push_back(new RevoluteRobotJoint(position_sensors_[1], 0.17 / 2.5, actuators_[1], 0.17 / 2.5)); // forearm pronation/supination
    robot_joints_.push_back(new PrismaticRobotJoint(position_sensors_[2], INCH2METER*0.23, actuators_[2], INCH2METER*0.23)); // wrist prismatic l_1
    robot_joints_.push_back(new PrismaticRobotJoint(position_sensors_[3], INCH2METER*0.23, actuators_[3], INCH2METER*0.23)); // wrist prismatic l_2
    robot_joints_.push_back(new PrismaticRobotJoint(position_sensors_[4], INCH2METER*0.23, actuators_[4], INCH2METER*0.23)); // wrist prismatic l_3
    */
    /*
    joints_.push_back(new RevoluteJoint()); // wrist revolute theta_1
    joints_.push_back(new RevoluteJoint()); // wrist revolute theta_2
    joints_.push_back(new RevoluteJoint()); // wrist revolute theta_3
    joints_.push_back(new PrismaticJoint()); // wrist platform position x_c
    joints_.push_back(new PrismaticJoint()); // wrist platform position y_c
    joints_.push_back(new PrismaticJoint()); // wrist platform position z_c
    joints_.push_back(new RevoluteJoint()); // wrist flexion/extension alpha
    joints_.push_back(new RevoluteJoint()); // wrist radial/ulnar deviation beta
    joints_.push_back(new RevoluteJoint()); // wrist platform roll gamma
    */
    //joint_positions_ = double_vec(joints_.size(), 0.0);

    //qp_ << -PI / 4, -PI / 4, -PI / 4, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0, 0, 0.1305;
                


/*
double_vec MahiExoII::get_anatomical_joint_positions() {

    // get positions from first two anatomical joints, which have encoders
    anatomical_joint_positions_[0] = robot_joints_[0]->get_position;
    anatomical_joint_positions_[1] = robot_joints_[1]->get_position;

    // update qs_ (q star) with the three prismatic link positions
    qs_ << robot_joints_[2]->get_position, robot_joints_[3]->get_position, robot_joints_[4]->get_position;

    // run forward kinematics solver to update qp_ (q prime), which contains all 12 RPS positions
    forward_kinematics(qs_, 10, 0.00000000001);

    // get positions for last two anatomical joints from forward kinematics solver
    anatomical_joint_positions_[2] = qp_(6);
    anatomical_joint_positions_[3] = qp_(7);

    return anatomical_joint_positions_;
}

double_vec MahiExoII::get_anatomical_joint_velocities() {

    // get velocities from first two anatomical joints, which have encoders
    anatomical_joint_velocities_[0] = robot_joints_[0]->get_velocity;
    anatomical_joint_velocities_[1] = robot_joints_[1]->get_velocity;

    // update qs_dot_ (q star time derivative) with the three prismatic link velocities
    qs_dot_ << robot_joints_[2]->get_velocity, robot_joints_[3]->get_velocity, robot_joints_[4]->get_velocity;

    // run forward kinematics solver to update qp_dot_ (q prime time derivative), which contains all 12 RPS velocities
    forward_kinematics_velocity(qs_dot_);

    // get positions for last two anatomical joints from forward kinematics solver
    anatomical_joint_velocities_[2] = qp_(6);
    anatomical_joint_velocities_[3] = qp_(7);

    return anatomical_joint_velocities_;
}

void MahiExoII::set_anatomical_joint_torques(double_vec new_torques) {


}
*/
    
/*
double_vec MahiExoII::get_joint_positions() {

    // get joint positions from first five joints, which have encoders
    for (auto it = 0; it != 5; ++it) {
        joints_[it]->get_position();
    }
        
    // update qs_ (q star) with the three prismatic link positions
    qs_ << joints_[2]->position_, joints_[3]->position_, joints_[4]->position_;

    // run forward kinematics solver to update qp_ (q prime), which contains all 12 RPS positions
    forward_kinematics(qs_, 10, 0.00000000001);

    // update joint positions solved for with FK solver
    joints_[5]->position_ = qp_[0]; // theta_1
    joints_[6]->position_ = qp_[1]; // theta_2
    joints_[7]->position_ = qp_[2]; // theta_3
    joints_[8]->position_ = qp_[6]; // alpha
    joints_[9]->position_ = qp_[7]; // beta
    joints_[10]->position_ = qp_[8]; // gamma
    joints_[11]->position_ = qp_[9]; // x_c
    joints_[12]->position_ = qp_[10]; // y_c
    joints_[13]->position_ = qp_[11]; // z_c

    // update and return exo joint vector
    for (auto it = joints_.begin(); it != joints_.end(); ++it) {
        joint_positions_[it - joints_.begin()] = joints_[it - joints_.begin()]->position_;
    }
    return joint_positions_;
}
    

void MahiExoII::forward_kinematics(Eigen::VectorXd qs, uint32 max_it, double tol) {
        
    // declare and initialize variables for keeping track of error
    double err = 2*tol;
    double a = 0;
    double b = 0;
    double c = 0;
    bool first_non_zero = true;

    // run no more than max_it iterations of updating the solution for qp_
    // exit loop once the error is below the input tolerance
    uint32 it = 0;
    while (it < max_it && err > tol) {
        psi_func(qp_, qs_, R_, r_, a56_, alpha5_, alpha13_); // actually psi bar, which evaluates 9 constraints and three desired values
        psi_d_qp_func(qp_, r_, alpha5_, alpha13_); // derivative of psi w.r.t. qp, giving a 12x12 matrix
        sol_ = psi_d_qp_.fullPivLu().solve(psi_);
        qp_ -= sol_;

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

        //std::cout << err << std::endl;


        // while iterator
        it++;
    }

        
    //std::cout << qp_.transpose() << std::endl;

}

void MahiExoII::psi_func(Eigen::VectorXd qp, Eigen::VectorXd qs, double R, double r, double a56, double alpha5, double alpha13) {
    psi_ << qp[3] * sin(qp[0]) - qp[9] - r*cos(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R*cos(alpha5) - qp[10] - a56*sin(alpha5) - qp[3] * cos(alpha5)*cos(qp[0]) - r*cos(alpha13)*cos(qp[7])*cos(qp[8]) + r*cos(qp[7])*sin(alpha13)*sin(qp[8]),
        a56*cos(alpha5) - qp[11] + R*sin(alpha5) - qp[3] * sin(alpha5)*cos(qp[0]) - r*cos(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[4] * sin(qp[1]) - qp[9] - r*cos(alpha13 - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R*cos(alpha5 - (2 * PI) / 3) - qp[10] - a56*sin(alpha5 - (2 * PI) / 3) - qp[4] * cos(alpha5 - (2 * PI) / 3)*cos(qp[1]) - r*cos(qp[7])*cos(qp[8])*cos(alpha13 - (2 * PI) / 3) + r*cos(qp[7])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3),
        a56*cos(alpha5 - (2 * PI) / 3) - qp[11] + R*sin(alpha5 - (2 * PI) / 3) - qp[4] * cos(qp[1])*sin(alpha5 - (2 * PI) / 3) - r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[5] * sin(qp[2]) - qp[9] - r*cos((2 * PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin((2 * PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R*cos((2 * PI) / 3 + alpha5) - qp[10] - a56*sin((2 * PI) / 3 + alpha5) - qp[5] * cos((2 * PI) / 3 + alpha5)*cos(qp[2]) - r*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13) + r*cos(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13),
        a56*cos((2 * PI) / 3 + alpha5) - qp[11] + R*sin((2 * PI) / 3 + alpha5) - qp[5] * cos(qp[2])*sin((2 * PI) / 3 + alpha5) - r*cos((2 * PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin((2 * PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[3] - qs[0],
        qp[4] - qs[1],
        qp[5] - qs[2];
}
    
void MahiExoII::psi_d_qp_func(Eigen::VectorXd qp, double r, double alpha5, double alpha13) {
    psi_d_qp_ << qp[3] * cos(qp[0]), 0, 0, sin(qp[0]), 0, 0, -r*cos(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[6])*cos(alpha13)*cos(qp[7])*cos(qp[8]) - r*cos(qp[6])*cos(qp[7])*sin(alpha13)*sin(qp[8]), r*sin(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                qp[3] * cos(alpha5)*sin(qp[0]), 0, 0, -cos(alpha5)*cos(qp[0]), 0, 0, 0, r*cos(alpha13)*cos(qp[8])*sin(qp[7]) - r*sin(alpha13)*sin(qp[7])*sin(qp[8]), r*cos(alpha13)*cos(qp[7])*sin(qp[8]) + r*cos(qp[7])*cos(qp[8])*sin(alpha13), 0, -1, 0,
                qp[3] * sin(alpha5)*sin(qp[0]), 0, 0, -sin(alpha5)*cos(qp[0]), 0, 0, r*cos(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*sin(alpha13)*sin(qp[8]) - r*cos(alpha13)*cos(qp[7])*cos(qp[8])*sin(qp[6]), r*sin(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                0, qp[4] * cos(qp[1]), 0, 0, sin(qp[1]), 0, -r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos(alpha13 - (2 * PI) / 3) - r*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3), r*sin(alpha13 - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                0, qp[4] * cos(alpha5 - (2 * PI) / 3)*sin(qp[1]), 0, 0, -cos(alpha5 - (2 * PI) / 3)*cos(qp[1]), 0, 0, r*cos(qp[8])*cos(alpha13 - (2 * PI) / 3)*sin(qp[7]) - r*sin(qp[7])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3), r*cos(qp[7])*cos(qp[8])*sin(alpha13 - (2 * PI) / 3) + r*cos(qp[7])*cos(alpha13 - (2 * PI) / 3)*sin(qp[8]), 0, -1, 0,
                0, qp[4] * sin(alpha5 - (2 * PI) / 3)*sin(qp[1]), 0, 0, -cos(qp[1])*sin(alpha5 - (2 * PI) / 3), 0, r*cos(alpha13 - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3) - r*cos(qp[7])*cos(qp[8])*cos(alpha13 - (2 * PI) / 3)*sin(qp[6]), r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                0, 0, qp[5] * cos(qp[2]), 0, 0, sin(qp[2]), -r*cos((2 * PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin((2 * PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13) - r*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13), r*sin((2 * PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos((2 * PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                0, 0, qp[5] * cos((2 * PI) / 3 + alpha5)*sin(qp[2]), 0, 0, -cos((2 * PI) / 3 + alpha5)*cos(qp[2]), 0, r*cos(qp[8])*cos((2 * PI) / 3 + alpha13)*sin(qp[7]) - r*sin(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13), r*cos(qp[7])*cos(qp[8])*sin((2 * PI) / 3 + alpha13) + r*cos(qp[7])*cos((2 * PI) / 3 + alpha13)*sin(qp[8]), 0, -1, 0,
                0, 0, qp[5] * sin((2 * PI) / 3 + alpha5)*sin(qp[2]), 0, 0, -cos(qp[2])*sin((2 * PI) / 3 + alpha5), r*cos((2 * PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin((2 * PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin((2 * PI) / 3 + alpha13) - r*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13)*sin(qp[6]), r*sin((2 * PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos((2 * PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0;
}
*/

