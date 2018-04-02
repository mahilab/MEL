#include "MEL/Exoskeletons/MahiExoII/MahiExoII.hpp"
#include "MEL/Daq/Quanser/Q8Usb.hpp"
#include "MEL/Math/Functions.hpp"
#include <MEL/Utility/Timer.hpp>
#include <MEL/Core/PositionSensor.hpp>
#include <MEL/Core/VelocitySensor.hpp>
#include <iomanip>
#include <MEL/Utility/Console.hpp>


namespace mel {

// geometric parameters
const double MahiExoII::R_ = 0.1044956;
const double MahiExoII::r_ = 0.05288174521;
const double MahiExoII::a56_ = 0.0268986 - 0.0272820;
const double MahiExoII::alpha5_ = 0.094516665054824;
const double MahiExoII::alpha13_ = 5 * DEG2RAD;


//-------------------------------------------------------------------------
// CONSTRUCTOR / DESTRUCTOR
//-------------------------------------------------------------------------

MahiExoII::MahiExoII(MeiiConfiguration configuration, MeiiParameters parameters) :
    Exo("mahi_exo_ii"),
    config_(configuration),
    params_(parameters)
{
    motors_.reserve(N_rj_);

    for (int i = 0; i < N_rj_; ++i) {

        std::string num = std::to_string(i);

        // construct motors
        motors_.push_back(Motor("meii_motor_" + num,
            params_.kt_[i],
            config_.amplifiers_[i],
            Limiter(params_.motor_cont_limits_[i],
                params_.motor_peak_limits_[i],
                params_.motor_i2t_times_[i])));

        // set encoder counts
        config_.encoder_channels_[i].set_units_per_count(2 * PI / params_.encoder_res_[i]);
        config_.velocity_channels_[i].set_units_per_count(2 * PI / params_.encoder_res_[i]);

        // construct joints
        Joint joint(
            "meii_joint_" + num,
            motors_[i],
            params_.eta_[i],
            config_.encoder_channels_[i],
            params_.eta_[i],
            config_.velocity_channels_[i],
            params_.eta_[i],
            std::array<double, 2>({ params_.pos_limits_min_[i] , params_.pos_limits_max_[i] }),
            params_.vel_limits_[i],
            params_.joint_torque_limits[i]);

        add_joint(joint);
    }

    for (int i = 0; i < N_aj_; i++) {
        anatomical_joint_positions_.push_back(0.0);
        anatomical_joint_velocities_.push_back(0.0);
        anatomical_joint_torques_.push_back(0.0);
    }

    rps_init_par_ref_ = SmoothReferenceTrajectory(rps_par_joint_speed_, rps_init_pos_);
    rps_par_ref_ = SmoothReferenceTrajectory(rps_par_joint_speed_);
    rps_ser_ref_ = SmoothReferenceTrajectory(rps_ser_joint_speed_);

    robot_ref_ = SmoothReferenceTrajectory(robot_joint_speed_);
    anat_ref_ = SmoothReferenceTrajectory(anat_joint_speed_);

}

MahiExoII::~MahiExoII() {
    if (is_enabled()) {
        disable();
    }
}

//-----------------------------------------------------------------------------
// PUBLIC FUNCTIONS
//-----------------------------------------------------------------------------

void MahiExoII::calibrate(volatile std::atomic<bool>& stop) {

    //enable DAQ
    config_.daq_.enable();
    std::vector<int32> encoder_offsets = { 0, -33259, 29125, 29125, 29125 };
    for (int i = 0; i < N_rj_; i++) {
        config_.encoder_channels_[i].reset_count(encoder_offsets[i]);
    }
    config_.daq_.disable();
}


bool MahiExoII::disable() {

    // disable reference trajectories
    rps_init_par_ref_.stop();
    rps_par_ref_.stop();
    rps_ser_ref_.stop();
    robot_ref_.stop();
    anat_ref_.stop();

    return Robot::disable();
}

//-----------------------------------------------------------------------------
// PUBLIC POSITION CONTROL FUNCTIONS
//-----------------------------------------------------------------------------

void MahiExoII::set_rps_control_mode(int mode) {
    switch (mode) {
    case 0: rps_control_mode_ = 0;
        break;
    case 1: rps_control_mode_ = 1;
        break;
    case 2: rps_control_mode_ = 2;
        break;
    default: print("WARNING: Invalid input argument to set_rps_control_mode(). Must be 0, 1, or 2. No change was made.");
    }
}

void MahiExoII::set_rps_backdrive(bool backdrive) {
    rps_backdrive_ = backdrive;
}

void MahiExoII::set_elbow_backdrive(bool backdrive) {
    elbow_backdrive_ = backdrive;
}

void MahiExoII::set_forearm_backdrive(bool backdrive) {
    forearm_backdrive_ = backdrive;
}

void MahiExoII::SmoothReferenceTrajectory::start(std::vector<double> current_pos, Time current_time) {        
    if (ref_init_) {
        started_ = true;
        prev_ref_ = current_pos;
        start_time_ = current_time;
    }
    else {
        print("ERROR: Reference position was not initialized. Must provide reference position to start().");
    }  
}

void MahiExoII::SmoothReferenceTrajectory::start(std::vector<double> ref_pos, std::vector<double> current_pos, Time current_time) {
    started_ = true;
    prev_ref_ = current_pos;
    ref_ = ref_pos;
    start_time_ = current_time;
}

void MahiExoII::SmoothReferenceTrajectory::set_ref(std::vector<double> ref_pos, Time current_time) {   
    if (!started_) {
        print("ERROR: Cannot call set_ref() before start().");
    }
    else {
        for (int i = 0; i < ref_pos.size(); ++i) {
            prev_ref_[i] = calculate_smooth_ref(i, current_time);
        }
        ref_ = ref_pos;
        start_time_ = current_time;
    }
}

double MahiExoII::SmoothReferenceTrajectory::calculate_smooth_ref(int dof, Time current_time) {
    if (started_) {
        if (ref_[dof] == prev_ref_[dof]) {
            return ref_[dof];
        }
        return prev_ref_[dof] + (ref_[dof] - prev_ref_[dof]) * saturate((current_time.as_seconds() - start_time_.as_seconds()) * speed_[dof] / std::abs(ref_[dof] - prev_ref_[dof]), 0.0, 1.0);
    }
    else {
        print("ERROR: Must give reference point first.");
        return NAN;
    }
}

void MahiExoII::SmoothReferenceTrajectory::stop() {
    started_ = false;
}


std::vector<double> MahiExoII::set_rps_pos_ctrl_torques(SmoothReferenceTrajectory& rps_ref, Time current_time) {

    std::vector<double> command_torques(N_qs_, 0.0);

    switch (rps_control_mode_) {
    case 0: // control impedance of parallel joints
        for (int i = 0; i < N_qs_; ++i) {
            if (rps_backdrive_) {
                command_torques[i] = 0.0;
            }
            else {
                double smooth_ref = rps_ref.calculate_smooth_ref(i, current_time);
                    
                if (std::isnan(smooth_ref)) {
                    command_torques[i] = 0.0;
                }
                else {
                    command_torques[i] = robot_joint_pd_controllers_[i + 2].calculate(smooth_ref, joints_[i + 2].get_position(), 0, joints_[i + 2].get_velocity());
                }
            }
        }
        set_rps_par_torques(command_torques);
        break;
    case 1: // control impedance of serial joints with platform height backdrivable
                
        for (int i = 0; i < N_qs_; ++i) {
            if (rps_backdrive_) {
                command_torques[i] = 0.0;
            }
            else {
                double smooth_ref = rps_ref.calculate_smooth_ref(i, current_time);
                if (std::isnan(smooth_ref)) {
                    command_torques[i] = 0.0;
                }
                else { 
                    command_torques[i] = anatomical_joint_pd_controllers_[i + 2].calculate(smooth_ref, get_anatomical_joint_position(i+2), 0, get_anatomical_joint_velocity(i+2));
                    command_torques[2] = 0.0; // set platform height commanded force to zero
                }
            }
        }
        set_rps_ser_torques(command_torques);
        break;

    case 2: // control impedance of serial joints with all joints active

        for (int i = 0; i < N_qs_; ++i) {
            if (rps_backdrive_) {
                command_torques[i] = 0.0;
            }
            else {
                double smooth_ref = rps_ref.calculate_smooth_ref(i, current_time);
                if (std::isnan(smooth_ref)) {
                    command_torques[i] = 0.0;
                }
                else {
                    command_torques[i] = anatomical_joint_pd_controllers_[i + 2].calculate(smooth_ref, get_anatomical_joint_position(i + 2), 0, get_anatomical_joint_velocity(i + 2));
                }
            }
        }
        set_rps_ser_torques(command_torques);
        break;

    default: print("WARNING: Invalid rps_control_mode_. Must be 0 or 1. Zero torques commanded.");
        for (int i = 0; i < N_qs_; ++i) {
            joints_[i + 2].set_torque(0.0);
        }
    }

    return command_torques;
   
}

std::vector<double> MahiExoII::set_anat_pos_ctrl_torques(SmoothReferenceTrajectory& anat_ref, Time current_time) {

    std::vector<double> command_torques(N_aj_, 0.0);

    // elbow joint
    if (elbow_backdrive_) {
        command_torques[0] = 0.0;
    }
    else {
        double smooth_ref = anat_ref.calculate_smooth_ref(0, current_time);
        if (std::isnan(smooth_ref)) {
            command_torques[0] = 0.0;
        }
        else {
            command_torques[0] = robot_joint_pd_controllers_[0].calculate(smooth_ref, joints_[0].get_position(), 0, joints_[0].get_velocity());
        }
    }

    // forearm joint
    if (forearm_backdrive_) {
        command_torques[1] = 0.0;
    }
    else {
        double smooth_ref = anat_ref.calculate_smooth_ref(1, current_time);
        if (std::isnan(smooth_ref)) {
            command_torques[1] = 0.0;
        }
        else {
            command_torques[1] = robot_joint_pd_controllers_[1].calculate(smooth_ref, joints_[1].get_position(), 0, joints_[1].get_velocity());
        }
    }
            

    // rps mechanism
    std::vector<double> rps_command_torques(N_qs_, 0.0);
    switch (rps_control_mode_) {
    case 1: // control impedance of serial joints with platform height backdrivable

        for (int i = 0; i < N_qs_; ++i) {
            if (rps_backdrive_) {
                rps_command_torques[i] = 0.0;
            }
            else {
                double smooth_ref = anat_ref.calculate_smooth_ref(i+2, current_time);
                if (std::isnan(smooth_ref)) {
                    rps_command_torques[i] = 0.0;
                }
                else {
                    rps_command_torques[i] = anatomical_joint_pd_controllers_[i+2].calculate(smooth_ref, get_anatomical_joint_position(i + 2), 0, get_anatomical_joint_velocity(i + 2));
                    rps_command_torques[2] = 0.0; // set platform height commanded force to zero
                }
            }
        }
        break;

    case 2: // control impedance of serial joints with all joints active

        for (int i = 0; i < N_qs_; ++i) {
            if (rps_backdrive_) {
                rps_command_torques[i] = 0.0;
            }
            else {
                double smooth_ref = anat_ref.calculate_smooth_ref(i + 2, current_time);
                if (std::isnan(smooth_ref)) {
                    rps_command_torques[i] = 0.0;
                }
                else {
                    rps_command_torques[i] = anatomical_joint_pd_controllers_[i+2].calculate(smooth_ref, get_anatomical_joint_position(i + 2), 0, get_anatomical_joint_velocity(i + 2));
                }
            }
        }
        break;

    default: print("WARNING: Invalid rps_control_mode_. Must be 1. Zero torques commanded.");
        for (auto i = 0; i < N_qs_; ++i) {
            rps_command_torques = std::vector<double>(N_qs_, 0.0);
        }
    }
    std::copy(rps_command_torques.begin(), rps_command_torques.end(), command_torques.begin() + 2);
    set_anatomical_joint_torques(command_torques);

    return command_torques;
}

        

//-----------------------------------------------------------------------------
// PUBLIC KINEMATICS FUNCTIONS
//-----------------------------------------------------------------------------

void MahiExoII::update_kinematics() {

    // update q_par_ (q parallel) with the three prismatic link positions
    q_par_ << joints_[2].get_position(), joints_[3].get_position(), joints_[4].get_position();
    q_par_dot_ << joints_[2].get_velocity(), joints_[3].get_velocity(), joints_[4].get_velocity();

    // run forward kinematics solver to update q_ser (q serial) and qp_ (q prime), which contains all 12 RPS positions
    forward_rps_kinematics_velocity(q_par_, q_ser_, qp_, rho_fk_, jac_fk_, q_par_dot_, q_ser_dot_, qp_dot_);

    // get positions from first two anatomical joints, which have encoders
    anatomical_joint_positions_[0] = joints_[0].get_position(); // elbow flexion/extension
    anatomical_joint_positions_[1] = joints_[1].get_position(); // forearm pronation/supination

    // get positions from forward kinematics solver for three wrist anatomical joints 
    anatomical_joint_positions_[2] = q_ser_[0]; // wrist flexion/extension
    anatomical_joint_positions_[3] = q_ser_[1]; // wrist radial/ulnar deviation
    anatomical_joint_positions_[4] = q_ser_[2]; // arm translation

    // get velocities from first two anatomical joints, which have encoders
    anatomical_joint_velocities_[0] = joints_[0].get_velocity(); // elbow flexion/extension
    anatomical_joint_velocities_[1] = joints_[1].get_velocity(); // forearm pronation/supination

    // get velocities from forward kinematics solver for three wrist anatomical joints 
    anatomical_joint_velocities_[2] = q_ser_dot_[0]; // wrist flexion/extension
    anatomical_joint_velocities_[3] = q_ser_dot_[1]; // wrist radial/ulnar deviation
    anatomical_joint_velocities_[4] = q_ser_dot_[2]; // arm translation
}

std::vector<double> MahiExoII::get_wrist_parallel_positions() const {
    return copy_eigvec_to_stdvec(q_par_);
}

std::vector<double> MahiExoII::get_wrist_serial_positions() const {
    return copy_eigvec_to_stdvec(q_ser_);
}

void MahiExoII::set_anatomical_joint_torques(std::vector<double> new_torques) {

    // set torques for first two anatomical joints, which have actuators
    joints_[0].set_torque(new_torques[0]);
    joints_[1].set_torque(new_torques[1]);


    // calculate the spectral norm of the transformation matrix
    Eigen::EigenSolver<Eigen::Matrix3d> eigensolver(jac_fk_.transpose() * jac_fk_, false);
    if (eigensolver.info() != Eigen::Success) {
        joints_[2].set_torque(0.0);
        joints_[3].set_torque(0.0);
        joints_[4].set_torque(0.0);
        //error_code_ = -1;
    }
    Eigen::EigenSolver<Eigen::Matrix3d>::EigenvalueType lambda = eigensolver.eigenvalues();
    std::vector<double> lambda_abs;
    for (int i = 0; i < N_qs_; ++i) {
        lambda_abs.push_back(std::abs(lambda(i)));
    }
    std::vector<double>::iterator lambda_max;
    lambda_max = std::max_element(lambda_abs.begin(), lambda_abs.end());
    double spec_norm = std::sqrt(*lambda_max);
    //print(spec_norm);

    // kill robot if norm too large
    if (spec_norm > 100) {
        //error_code_ = -3;
    }

    if (spec_norm_prev_ != 0) {
        if (std::abs(spec_norm - spec_norm_prev_) > 100) {
            std::cout << std::setprecision(12);
            std::cout << q_par_prev_.transpose() << std::endl;
            std::cout << q_par_.transpose() << std::endl;
            std::cout << qp_.transpose() << std::endl;
            std::cout << jac_fk_ << std::endl;
            //mel::print(lambda_abs);
            //error_code_ = -2;
        }
    }

    // debugging
    spec_norm_prev_ = spec_norm;
    q_par_prev_ = q_par_;

    // set torques for two wrist anatomical joints and arm translation
    Eigen::VectorXd par_torques = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd ser_torques = Eigen::VectorXd::Zero(N_qs_);
    ser_torques(0) = new_torques[2];
    ser_torques(1) = new_torques[3];
    ser_torques(2) = new_torques[4];
    par_torques = jac_fk_.transpose()*ser_torques;
    joints_[2].set_torque(par_torques(0));
    joints_[3].set_torque(par_torques(1));
    joints_[4].set_torque(par_torques(2));

    // store parallel and serial joint torques for data logging
    tau_par_rob_ = par_torques;
    tau_ser_rob_ = -ser_torques;
}


void MahiExoII::set_rps_par_torques(std::vector<double>& tau_par) {
    for (int i = 0; i < N_qs_; ++i) {
        joints_[i + 2].set_torque(tau_par[i]);
    }
        
    //tau_par_rob_ = copy_stdvec_to_eigvec(tau_par);
    //std::cout << tau_par_rob_.transpose() << std::endl;
    //print("");

    //Eigen::VectorXd tau_b(N_qp_ - N_qs_);
    //tau_b << 0.0, 0.0, 0.0, tau_par_rob_[0], tau_par_rob_[1], tau_par_rob_[2], 0.0, 0.0, 0.0;
    //solve_static_rps_torques(select_q_ser_, tau_b, qp_, tau_ser_rob_);
}

void MahiExoII::set_rps_ser_torques(std::vector<double>& tau_ser) {
    Eigen::VectorXd tau_ser_eig = copy_stdvec_to_eigvec(tau_ser);
    //Eigen::VectorXd tau_par = Eigen::VectorXd::Zero(N_qs_);
    //tau_par = jac_fk_.transpose() * tau_ser_eig;
    tau_par_rob_ = jac_fk_.transpose() * tau_ser_eig;
    for (int i = 0; i < N_qs_; ++i) {
        //joints_[i + 2]->set_torque(tau_par[i]);
        joints_[i + 2].set_torque(tau_par_rob_[i]);
    }
    tau_ser_rob_ = -tau_ser_eig;
}


void MahiExoII::forward_rps_kinematics(std::vector<double>& q_par_in, std::vector<double>& q_ser_out) const {
    std::vector<double> qp;
    forward_rps_kinematics(q_par_in, q_ser_out, qp);
}

void MahiExoII::forward_rps_kinematics(std::vector<double>& q_par_in, std::vector<double>& q_ser_out, std::vector<double>& qp_out) const {

    Eigen::VectorXd q_par_in_eig = copy_stdvec_to_eigvec(q_par_in);
    Eigen::VectorXd q_ser_out_eig = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);

    forward_rps_kinematics(q_par_in_eig, q_ser_out_eig, qp_out_eig);

    q_ser_out = copy_eigvec_to_stdvec(q_ser_out_eig);
    qp_out = copy_eigvec_to_stdvec(qp_out_eig);

}

void MahiExoII::forward_rps_kinematics_velocity(std::vector<double>& q_par_in, std::vector<double>& q_ser_out, std::vector<double>& q_par_dot_in, std::vector<double>& q_ser_dot_out) const {
    std::vector<double> qp, qp_dot;
    forward_rps_kinematics_velocity(q_par_in, q_ser_out, qp, q_par_dot_in, q_ser_dot_out, qp_dot);
}


void MahiExoII::forward_rps_kinematics_velocity(std::vector<double>& q_par_in, std::vector<double>& q_ser_out, std::vector<double>& qp_out, std::vector<double>& q_par_dot_in, std::vector<double>& q_ser_dot_out, std::vector<double>& qp_dot_out) const {

    Eigen::VectorXd q_par_in_eig = copy_stdvec_to_eigvec(q_par_in);
    Eigen::VectorXd q_ser_out_eig = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);
    Eigen::VectorXd q_par_dot_in_eig = copy_stdvec_to_eigvec(q_par_dot_in);
    Eigen::VectorXd q_ser_dot_out_eig = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp_dot_out_eig = Eigen::VectorXd::Zero(N_qs_);

    forward_rps_kinematics_velocity(q_par_in_eig, q_ser_out_eig, qp_out_eig, q_par_dot_in_eig, q_ser_dot_out_eig, qp_dot_out_eig);

    q_ser_out = copy_eigvec_to_stdvec(q_ser_out_eig);
    qp_out = copy_eigvec_to_stdvec(qp_out_eig);
    q_ser_dot_out = copy_eigvec_to_stdvec(q_ser_dot_out_eig);
    qp_dot_out = copy_eigvec_to_stdvec(qp_dot_out_eig);

}

void MahiExoII::inverse_rps_kinematics(std::vector<double>& q_ser_in, std::vector<double>& q_par_out) const {
    std::vector<double> qp;
    inverse_rps_kinematics(q_ser_in, q_par_out, qp);
}

void MahiExoII::inverse_rps_kinematics(std::vector<double>& q_ser_in, std::vector<double>& q_par_out, std::vector<double>& qp_out) const {

    Eigen::VectorXd q_ser_in_eig = copy_stdvec_to_eigvec(q_ser_in);
    Eigen::VectorXd q_par_out_eig = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);

    inverse_rps_kinematics(q_ser_in_eig, q_par_out_eig, qp_out_eig);

    q_par_out = copy_eigvec_to_stdvec(q_par_out_eig);
    qp_out = copy_eigvec_to_stdvec(qp_out_eig);

}

void MahiExoII::inverse_rps_kinematics_velocity(std::vector<double>& q_ser_in, std::vector<double>& q_par_out, std::vector<double>& q_ser_dot_in, std::vector<double>& q_par_dot_out) const {
    std::vector<double> qp, qp_dot;
    inverse_rps_kinematics_velocity(q_ser_in, q_par_out, qp, q_ser_dot_in, q_par_dot_out, qp_dot);
}

void MahiExoII::inverse_rps_kinematics_velocity(std::vector<double>& q_ser_in, std::vector<double>& q_par_out, std::vector<double>& qp_out, std::vector<double>& q_ser_dot_in, std::vector<double>& q_par_dot_out, std::vector<double>& qp_dot_out) const {

    Eigen::VectorXd q_ser_in_eig = copy_stdvec_to_eigvec(q_ser_in);
    Eigen::VectorXd q_par_out_eig = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);
    Eigen::VectorXd q_ser_dot_in_eig = copy_stdvec_to_eigvec(q_ser_dot_in);
    Eigen::VectorXd q_par_dot_out_eig = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp_dot_out_eig = Eigen::VectorXd::Zero(N_qp_);

    inverse_rps_kinematics_velocity(q_ser_in_eig, q_par_out_eig, qp_out_eig, q_ser_dot_in_eig, q_par_dot_out_eig, qp_dot_out_eig);

    q_par_out = copy_eigvec_to_stdvec(q_par_out_eig);
    qp_out = copy_eigvec_to_stdvec(qp_out_eig);
    q_par_dot_out = copy_eigvec_to_stdvec(q_par_dot_out_eig);
    qp_dot_out = copy_eigvec_to_stdvec(qp_dot_out_eig);

}

        
//-----------------------------------------------------------------------------
// PUBLIC UTILITY FUNCTIONS
//-----------------------------------------------------------------------------

bool MahiExoII::check_rps_init(bool print_output) const {
    std::vector<double> rps_pos_tol_vec(N_qs_, rps_init_err_tol_);
    return check_goal_pos(rps_init_pos_, get_wrist_parallel_positions(), { 1,1,1 }, rps_pos_tol_vec, print_output);
}

bool MahiExoII::check_goal_rps_par_pos(std::vector<double> goal_rps_par_pos, std::vector<char> check_dof, bool print_output) const {
    return check_goal_pos(goal_rps_par_pos, get_wrist_parallel_positions(), check_dof, rps_par_goal_err_tol_, print_output );
}

bool MahiExoII::check_goal_rps_ser_pos(std::vector<double> goal_rps_ser_pos, std::vector<char> check_dof, bool print_output) const {
    return check_goal_pos(goal_rps_ser_pos, get_wrist_serial_positions(), check_dof, rps_ser_goal_err_tol_, print_output);
}

bool MahiExoII::check_goal_anat_pos(std::vector<double> goal_anat_pos, std::vector<char> check_dof, bool print_output) const {
    return check_goal_pos(goal_anat_pos, get_anatomical_joint_positions(), check_dof, anat_goal_err_tol_, print_output);
}

bool MahiExoII::check_neutral_anat_pos(std::vector<double> goal_anat_pos, std::vector<char> check_dof, bool print_output) const {
    return check_goal_pos(goal_anat_pos, get_anatomical_joint_positions(), check_dof, anat_neutral_err_tol_, print_output);
}


//-----------------------------------------------------------------------------
// PUBLIC DATA LOGGING FUNCTIONS
//-----------------------------------------------------------------------------

/*void MahiExoII::init_robot_log() {
    robot_log_.add_col("Time [s]")
        .add_col("MEII Joint 0 Encoder Count [counts]").add_col("MEII Joint 0 Encoder Rate [counts/s]").add_col("MEII Joint 0 Motor Command Current [A]").add_col("MEII Joint 0 Motor Limited Current [A]")
        .add_col("MEII Joint 1 Encoder Count [counts]").add_col("MEII Joint 1 Encoder Rate [counts/s]").add_col("MEII Joint 1 Motor Command Current [A]").add_col("MEII Joint 1 Motor Limited Current [A]")
        .add_col("MEII Joint 2 Encoder Count [counts]").add_col("MEII Joint 2 Encoder Rate [counts/s]").add_col("MEII Joint 2 Motor Command Current [A]").add_col("MEII Joint 2 Motor Limited Current [A]")
        .add_col("MEII Joint 3 Encoder Count [counts]").add_col("MEII Joint 3 Encoder Rate [counts/s]").add_col("MEII Joint 3 Motor Command Current [A]").add_col("MEII Joint 3 Motor Limited Current [A]")
        .add_col("MEII Joint 4 Encoder Count [counts]").add_col("MEII Joint 4 Encoder Rate [counts/s]").add_col("MEII Joint 4 Motor Command Current [A]").add_col("MEII Joint 4 Motor Limited Current [A]")
        .add_col("MEII EFE Position [rad]").add_col("MEII EFE Velocity [rad/s]").add_col("MEII EFE Commanded Torque [Nm]")
        .add_col("MEII FPS Position [rad]").add_col("MEII FPS Velocity [rad/s]").add_col("MEII FPS Commanded Torque [Nm]")
        .add_col("MEII RPS Theta1 Position [rad]").add_col("MEII RPS Theta2 Position [rad]").add_col("MEII RPS Theta3 Position [rad]")
        .add_col("MEII RPS L1 Position [m]").add_col("MEII RPS L2 Position [m]").add_col("MEII RPS L3 Position [m]")
        .add_col("MEII RPS Alpha Position [rad]").add_col("MEII RPS Beta Position [rad]").add_col("MEII RPS Gamma Position [rad]")
        .add_col("MEII RPS X Position [m]").add_col("MEII RPS Y Position [m]").add_col("MEII RPS Z Position [m]")
        .add_col("MEII RPS Theta1 Velocity [rad/s]").add_col("MEII RPS Theta2 Velocity [rad/s]").add_col("MEII RPS Theta3 Velocity [rad/s]")
        .add_col("MEII RPS L1 Velocity [m/s]").add_col("MEII RPS L2 Velocity [m/s]").add_col("MEII RPS L3 Velocity [m/s]")
        .add_col("MEII RPS Alpha Velocity [rad/s]").add_col("MEII RPS Beta Velocity [rad/s]").add_col("MEII RPS Gamma Velocity [rad/s]")
        .add_col("MEII RPS X Velocity [m/s]").add_col("MEII RPS Y Velocity [m/s]").add_col("MEII RPS Z Velocity [m/s]")
        .add_col("MEII RPS L1 Force [N]").add_col("MEII RPS L2 Force [N]").add_col("MEII RPS L3 Force [N]")
        .add_col("MEII RPS Alpha Torque [Nm]").add_col("MEII RPS Beta Torque [Nm]").add_col("MEII RPS X Force [N]");      
}*/

/*void MahiExoII::log_robot_row(double time) {

    std::vector<double> row;
    row.push_back(time);
    row.push_back(static_cast<Encoder*>(joints_[0].position_sensor_)->get_encoder_counts());
    row.push_back(static_cast<Encoder*>(joints_[0].position_sensor_)->get_encoder_rate());
    row.push_back(static_cast<Motor*>(actuators_[0]).get_current_command());
    row.push_back(static_cast<Motor*>(actuators_[0]).get_current_limited());
    row.push_back(static_cast<Encoder*>(joints_[1].position_sensor_)->get_encoder_counts());
    row.push_back(static_cast<Encoder*>(joints_[1].position_sensor_)->get_encoder_rate());
    row.push_back(static_cast<Motor*>(actuators_[1]).get_current_command());
    row.push_back(static_cast<Motor*>(actuators_[1]).get_current_limited());
    row.push_back(static_cast<Encoder*>(joints_[2].position_sensor_)->get_encoder_counts());
    row.push_back(static_cast<Encoder*>(joints_[2].position_sensor_)->get_encoder_rate());
    row.push_back(static_cast<Motor*>(actuators_[2]).get_current_command());
    row.push_back(static_cast<Motor*>(actuators_[2]).get_current_limited());
    row.push_back(static_cast<Encoder*>(joints_[3].position_sensor_)->get_encoder_counts());
    row.push_back(static_cast<Encoder*>(joints_[3].position_sensor_)->get_encoder_rate());
    row.push_back(static_cast<Motor*>(actuators_[3]).get_current_command());
    row.push_back(static_cast<Motor*>(actuators_[3]).get_current_limited());
    row.push_back(static_cast<Encoder*>(joints_[4].position_sensor_)->get_encoder_counts());
    row.push_back(static_cast<Encoder*>(joints_[4].position_sensor_)->get_encoder_rate());
    row.push_back(static_cast<Motor*>(actuators_[4])->get_current_command());
    row.push_back(static_cast<Motor*>(actuators_[4])->get_current_limited());
    row.push_back(joints_[0].get_position());
    row.push_back(joints_[0].get_velocity());
    row.push_back(joints_[0].get_torque());
    row.push_back(joints_[1].get_position());
    row.push_back(joints_[1].get_velocity());
    row.push_back(joints_[1].get_torque());
    for (int i = 0; i < N_qp_; ++i) {
        row.push_back(qp_[i]);
    }
    for (int i = 0; i < N_qp_; ++i) {
        row.push_back(qp_dot_[i]);
    }
    for (int i = 0; i < N_qs_; ++i) {
        row.push_back(tau_par_rob_[i]);
    }
    for (int i = 0; i < N_qs_; ++i) {
        row.push_back(tau_ser_rob_[i]);
    }
    robot_log_.add_row(row);
}*/

/*void MahiExoII::save_and_clear_robot_log(std::string filename, std::string directory, bool timestamp) {
    robot_log_.save_and_clear_data(filename, directory, timestamp);
    robot_log_ = DataLog("robot_log", false);
}*/


//-----------------------------------------------------------------------------
// PRIVATE KINEMATICS FUNCTIONS
//-----------------------------------------------------------------------------


void MahiExoII::solve_static_rps_torques(std::vector<uint8> select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s) const {
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    generate_rho(select_q, qp, rho);
    tau_s = -rho.transpose() * tau_b;
}

void MahiExoII::solve_static_rps_torques(std::vector<uint8> select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s, Eigen::VectorXd& tau_p) const {
    solve_static_rps_torques(select_q, tau_b, qp, tau_s);
    for (int i = 0; i < N_qs_; ++i) {
        tau_p[select_q[i]] = tau_s[i];
    }
    std::vector<uint8> indices = select_q_invert(select_q);
    for (int i = 0; i < N_qp_ - N_qs_; ++i) {
        tau_p[indices.at(i)] = tau_b[i];
    }
}


void MahiExoII::forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    forward_rps_kinematics(q_par_in, q_ser_out, qp, rho, jac);
}

void MahiExoII::forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out) const {
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    forward_rps_kinematics(q_par_in, q_ser_out, qp_out, rho, jac);
}

void MahiExoII::forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::MatrixXd& jac_fk) const {
    Eigen::VectorXd q_ser = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    forward_rps_kinematics(q_par_in, q_ser, qp, rho, jac_fk);
}

void MahiExoII::forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::MatrixXd& jac_fk) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    forward_rps_kinematics(q_par_in, q_ser_out, qp, rho, jac_fk);
}

void MahiExoII::forward_rps_kinematics(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_fk, Eigen::MatrixXd& jac_fk) const {
    Eigen::MatrixXd rho_s = Eigen::MatrixXd::Zero(N_qp_, N_qs_);
    solve_rps_kinematics(select_q_par_, q_par_in, qp_out, rho_fk, rho_s, max_it_, tol_);
    q_ser_out << qp_out(select_q_ser_[0]), qp_out(select_q_ser_[1]), qp_out(select_q_ser_[2]);
    for (int i = 0; i < N_qs_; ++i) {
        jac_fk.row(i) = rho_s.row(select_q_ser_[i]);
    }
}

void MahiExoII::forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const {
    Eigen::VectorXd q_par = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd q_ser = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::VectorXd qp_dot = Eigen::VectorXd::Zero(N_qp_);
    forward_rps_kinematics_velocity(q_par, q_ser, qp, rho, jac, q_par_dot_in, q_ser_dot_out, qp_dot);
}

void MahiExoII::forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::VectorXd qp_dot = Eigen::VectorXd::Zero(N_qp_);
    forward_rps_kinematics_velocity(q_par_in, q_ser_out, qp, rho, jac, q_par_dot_in, q_ser_dot_out, qp_dot);
}

void MahiExoII::forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::MatrixXd& jac_fk, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::VectorXd qp_dot = Eigen::VectorXd::Zero(N_qp_);
    forward_rps_kinematics_velocity(q_par_in, q_ser_out, qp, rho, jac_fk, q_par_dot_in, q_ser_dot_out, qp_dot);
}

void MahiExoII::forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out) const {
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    forward_rps_kinematics_velocity(q_par_in, q_ser_out, qp_out, rho, jac, q_par_dot_in, q_ser_dot_out, qp_dot_out);
}

void MahiExoII::forward_rps_kinematics_velocity(const Eigen::VectorXd& q_par_in, Eigen::VectorXd& q_ser_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_fk, Eigen::MatrixXd& jac_fk, const Eigen::VectorXd& q_par_dot_in, Eigen::VectorXd& q_ser_dot_out, Eigen::VectorXd& qp_dot_out) const {
    forward_rps_kinematics(q_par_in, q_ser_out, qp_out, rho_fk, jac_fk);
    q_ser_dot_out = jac_fk * q_par_dot_in;
    Eigen::VectorXd qb_dot = Eigen::VectorXd::Zero(N_qp_ - N_qs_);
    qb_dot = rho_fk * q_par_dot_in;
    for (int i = 0; i < N_qs_; ++i) {
        qp_dot_out[select_q_ser_[i]] = q_ser_dot_out[i];
    }
    std::vector<uint8> indices = select_q_invert(select_q_ser_);
    for (int i = 0; i < N_qp_ - N_qs_; ++i) {
        qp_dot_out[indices[i]] = qb_dot[i];
    }
}    

void MahiExoII::inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    inverse_rps_kinematics(q_ser_in, q_par_out, qp, rho, jac);
}

void MahiExoII::inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out) const {
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    inverse_rps_kinematics(q_ser_in, q_par_out, qp_out, rho, jac);
}

void MahiExoII::inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::MatrixXd& jac_ik) const {
    Eigen::VectorXd q_par = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    inverse_rps_kinematics(q_ser_in, q_par, qp, rho, jac_ik);
}

void MahiExoII::inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::MatrixXd& jac_ik) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    inverse_rps_kinematics(q_ser_in, q_par_out, qp, rho, jac_ik);
}

void MahiExoII::inverse_rps_kinematics(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_ik, Eigen::MatrixXd& jac_ik) const {
    Eigen::MatrixXd rho_s = Eigen::MatrixXd::Zero(N_qp_, N_qs_);
    solve_rps_kinematics(select_q_ser_, q_ser_in, qp_out, rho_ik, rho_s, max_it_, tol_);
    q_par_out << qp_out(select_q_par_[0]), qp_out(select_q_par_[1]), qp_out(select_q_par_[2]);
    for (int i = 0; i < N_qs_; ++i) {
        jac_ik.row(i) = rho_s.row(select_q_par_[i]);
    }
}

void MahiExoII::inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const {
    Eigen::VectorXd q_ser = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd q_par = Eigen::VectorXd::Zero(N_qs_);
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::VectorXd qp_dot = Eigen::VectorXd::Zero(N_qp_);
    inverse_rps_kinematics_velocity(q_ser, q_par, qp, rho, jac, q_ser_dot_in, q_par_dot_out, qp_dot);
}

void MahiExoII::inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    Eigen::VectorXd qp_dot = Eigen::VectorXd::Zero(N_qp_);
    inverse_rps_kinematics_velocity(q_ser_in, q_par_out, qp, rho, jac, q_ser_dot_in, q_par_dot_out, qp_dot);
}

void MahiExoII::inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::MatrixXd& jac_ik, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out) const {
    Eigen::VectorXd qp = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::VectorXd qp_dot = Eigen::VectorXd::Zero(N_qp_);
    inverse_rps_kinematics_velocity(q_ser_in, q_par_out, qp, rho, jac_ik, q_ser_dot_in, q_par_dot_out, qp_dot);
}

void MahiExoII::inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out, Eigen::VectorXd& qp_dot_out) const {
    Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
    Eigen::MatrixXd jac = Eigen::MatrixXd::Zero(N_qs_, N_qs_);
    inverse_rps_kinematics_velocity(q_ser_in, q_par_out, qp_out, rho, jac, q_ser_dot_in, q_par_dot_out, qp_dot_out);
}

void MahiExoII::inverse_rps_kinematics_velocity(const Eigen::VectorXd& q_ser_in, Eigen::VectorXd& q_par_out, Eigen::VectorXd& qp_out, Eigen::MatrixXd& rho_ik, Eigen::MatrixXd& jac_ik, const Eigen::VectorXd& q_ser_dot_in, Eigen::VectorXd& q_par_dot_out, Eigen::VectorXd& qp_dot_out) const {
    inverse_rps_kinematics(q_ser_in, q_par_out, qp_out, rho_ik, jac_ik);
    q_par_dot_out = jac_ik * q_ser_dot_in;
    Eigen::VectorXd qb_dot = Eigen::VectorXd::Zero(N_qp_ - N_qs_);
    qb_dot = rho_ik * q_ser_dot_in;
    for (int i = 0; i < N_qs_; ++i) {
        qp_dot_out[select_q_par_.at(i)] = q_par_dot_out[i];
    }
    std::vector<uint8> indices = select_q_invert(select_q_par_);
    for (int i = 0; i < N_qp_ - N_qs_; ++i) {
        qp_dot_out[indices.at(i)] = qb_dot[i];
    }
}

void MahiExoII::solve_rps_kinematics(std::vector<uint8> select_q, const Eigen::VectorXd& qs, Eigen::VectorXd& qp, Eigen::MatrixXd& rho, Eigen::MatrixXd& rho_s, uint32 max_it, double tol) const {

    // build selection matrix
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N_qs_, N_qp_);
    for (int i = 0; i < N_qs_; ++i) {
        A(i, select_q[i]) = 1;
    }

    // initialize variable containing solution
    qp << PI / 4, PI / 4, PI / 4, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0.0923, 0, 0;

    // initialize temporary variables containing kinematic constraints etc.
    Eigen::VectorXd phi = Eigen::VectorXd::Zero(N_qp_ - N_qs_);
    Eigen::MatrixXd phi_d_qp = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qp_);
    Eigen::VectorXd psi = Eigen::VectorXd::Zero(N_qp_);
    Eigen::MatrixXd psi_d_qp = Eigen::MatrixXd::Zero(N_qp_, N_qp_);
    Eigen::MatrixXd rho_rhs = Eigen::MatrixXd::Zero(N_qp_, N_qs_);
    rho_rhs.bottomRows(N_qs_) = Eigen::MatrixXd::Identity(N_qs_, N_qs_);

    // initialize variables for keeping track of error
    double err = 2 * tol;
    double a = 0;
    double b = 0;
    double c = 0;
    bool first_non_zero = true;

    // run no more than max_it iterations of updating the solution for qp_
    // exit loop once the error is below the input tolerance
    uint32 it = 0;
    while (it < max_it && err > tol) {
        psi_update(A, qs, qp, phi, psi); // calculate 9 constraints and tracking error on specified qs_
        psi_d_qp_update(A, qp, phi_d_qp, psi_d_qp); // derivative of psi w.r.t. qp, giving a 12x12 matrix      
        qp -= psi_d_qp.fullPivLu().solve(psi);

        // update the error (don't know why it's like this, but it seems to work)
        err = 0;
        c = 0;
        first_non_zero = true;
        for (auto j = 0; j != N_qp_; ++j) {
            a = psi[j];
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

    rho_s = psi_d_qp.fullPivLu().solve(rho_rhs);

    // remove rows corresponding to q_select indices
    std::vector<uint8> indices = select_q_invert(select_q);
    for (int i = 0; i < N_qp_ - N_qs_; ++i) {
        rho.row(i) = rho_s.row(indices.at(i));
    }
}

void MahiExoII::generate_rho(std::vector<uint8> select_q, const Eigen::VectorXd& qp, Eigen::MatrixXd& rho) const {
            
    // build selection matrix
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N_qs_, N_qp_);
    for (int i = 0; i < N_qs_; ++i) {
        A(i, select_q[i]) = 1;
    }

    // initialize temporary variables containing kinematic constraints etc.
    Eigen::MatrixXd phi_d_qp = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qp_);
    Eigen::MatrixXd psi_d_qp = Eigen::MatrixXd::Zero(N_qp_, N_qp_);
    Eigen::MatrixXd rho_s = Eigen::MatrixXd::Zero(N_qp_, N_qs_);
    Eigen::MatrixXd rho_rhs = Eigen::MatrixXd::Zero(N_qp_, N_qs_);
    rho_rhs.bottomRows(N_qs_) = Eigen::MatrixXd::Identity(N_qs_, N_qs_);

    // compute rho_s
    psi_d_qp_update(A, qp, phi_d_qp, psi_d_qp);
    rho_s = psi_d_qp.fullPivLu().solve(rho_rhs);

    // remove rows corresponding to q_select indices
    std::vector<uint8> indices = select_q_invert(select_q);
    for (int i = 0; i < N_qp_ - N_qs_; ++i) {
        rho.row(i) = rho_s.row(indices.at(i));
    }

}

void MahiExoII::psi_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qs, const Eigen::VectorXd& qp, Eigen::VectorXd& phi, Eigen::VectorXd& psi) const {
    phi_update(qp, phi);
    psi.head(N_qp_ - N_qs_) = phi;
    psi.tail(N_qs_) = A*qp - qs;
}


void MahiExoII::psi_d_qp_update(const Eigen::MatrixXd& A, const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp, Eigen::MatrixXd& psi_d_qp) const {
    phi_d_qp_update(qp, phi_d_qp);
    psi_d_qp.block<N_qp_ - N_qs_, N_qp_>(0, 0) = phi_d_qp;
    psi_d_qp.block<N_qs_, N_qp_>(N_qp_ - N_qs_, 0) = A;
}

void MahiExoII::phi_update(const Eigen::VectorXd& qp, Eigen::VectorXd& phi) const {

    phi << qp[3] * sin(qp[0]) - qp[9] - r_*cos(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R_*cos(alpha5_) - qp[10] - a56_*sin(alpha5_) - qp[3] * cos(alpha5_)*cos(qp[0]) - r_*cos(alpha13_)*cos(qp[7])*cos(qp[8]) + r_*cos(qp[7])*sin(alpha13_)*sin(qp[8]),
        a56_*cos(alpha5_) - qp[11] + R_*sin(alpha5_) - qp[3] * sin(alpha5_)*cos(qp[0]) - r_*cos(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[4] * sin(qp[1]) - qp[9] - r_*cos(alpha13_ - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin(alpha13_ - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R_*cos(alpha5_ - (2 * PI) / 3) - qp[10] - a56_*sin(alpha5_ - (2 * PI) / 3) - qp[4] * cos(alpha5_ - (2 * PI) / 3)*cos(qp[1]) - r_*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * PI) / 3) + r_*cos(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * PI) / 3),
        a56_*cos(alpha5_ - (2 * PI) / 3) - qp[11] + R_*sin(alpha5_ - (2 * PI) / 3) - qp[4] * cos(qp[1])*sin(alpha5_ - (2 * PI) / 3) - r_*cos(alpha13_ - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_ - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
        qp[5] * sin(qp[2]) - qp[9] - r_*cos((2 * PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin((2 * PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
        R_*cos((2 * PI) / 3 + alpha5_) - qp[10] - a56_*sin((2 * PI) / 3 + alpha5_) - qp[5] * cos((2 * PI) / 3 + alpha5_)*cos(qp[2]) - r_*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13_) + r_*cos(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13_),
        a56_*cos((2 * PI) / 3 + alpha5_) - qp[11] + R_*sin((2 * PI) / 3 + alpha5_) - qp[5] * cos(qp[2])*sin((2 * PI) / 3 + alpha5_) - r_*cos((2 * PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin((2 * PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8]));
}

void MahiExoII::phi_d_qp_update(const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp) const {

    phi_d_qp << qp[3] * cos(qp[0]), 0, 0, sin(qp[0]), 0, 0, -r_*cos(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(alpha13_)*cos(qp[7])*cos(qp[8]) - r_*cos(qp[6])*cos(qp[7])*sin(alpha13_)*sin(qp[8]), r_*sin(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        qp[3] * cos(alpha5_)*sin(qp[0]), 0, 0, -cos(alpha5_)*cos(qp[0]), 0, 0, 0, r_*cos(alpha13_)*cos(qp[8])*sin(qp[7]) - r_*sin(alpha13_)*sin(qp[7])*sin(qp[8]), r_*cos(alpha13_)*cos(qp[7])*sin(qp[8]) + r_*cos(qp[7])*cos(qp[8])*sin(alpha13_), 0, -1, 0,
        qp[3] * sin(alpha5_)*sin(qp[0]), 0, 0, -sin(alpha5_)*cos(qp[0]), 0, 0, r_*cos(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(alpha13_)*sin(qp[8]) - r_*cos(alpha13_)*cos(qp[7])*cos(qp[8])*sin(qp[6]), r_*sin(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
        0, qp[4] * cos(qp[1]), 0, 0, sin(qp[1]), 0, -r_*cos(alpha13_ - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_ - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * PI) / 3) - r_*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * PI) / 3), r_*sin(alpha13_ - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos(alpha13_ - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        0, qp[4] * cos(alpha5_ - (2 * PI) / 3)*sin(qp[1]), 0, 0, -cos(alpha5_ - (2 * PI) / 3)*cos(qp[1]), 0, 0, r_*cos(qp[8])*cos(alpha13_ - (2 * PI) / 3)*sin(qp[7]) - r_*sin(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * PI) / 3), r_*cos(qp[7])*cos(qp[8])*sin(alpha13_ - (2 * PI) / 3) + r_*cos(qp[7])*cos(alpha13_ - (2 * PI) / 3)*sin(qp[8]), 0, -1, 0,
        0, qp[4] * sin(alpha5_ - (2 * PI) / 3)*sin(qp[1]), 0, 0, -cos(qp[1])*sin(alpha5_ - (2 * PI) / 3), 0, r_*cos(alpha13_ - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin(alpha13_ - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin(alpha13_ - (2 * PI) / 3) - r_*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * PI) / 3)*sin(qp[6]), r_*sin(alpha13_ - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos(alpha13_ - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
        0, 0, qp[5] * cos(qp[2]), 0, 0, sin(qp[2]), -r_*cos((2 * PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin((2 * PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13_) - r_*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13_), r_*sin((2 * PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos((2 * PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
        0, 0, qp[5] * cos((2 * PI) / 3 + alpha5_)*sin(qp[2]), 0, 0, -cos((2 * PI) / 3 + alpha5_)*cos(qp[2]), 0, r_*cos(qp[8])*cos((2 * PI) / 3 + alpha13_)*sin(qp[7]) - r_*sin(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13_), r_*cos(qp[7])*cos(qp[8])*sin((2 * PI) / 3 + alpha13_) + r_*cos(qp[7])*cos((2 * PI) / 3 + alpha13_)*sin(qp[8]), 0, -1, 0,
        0, 0, qp[5] * sin((2 * PI) / 3 + alpha5_)*sin(qp[2]), 0, 0, -cos(qp[2])*sin((2 * PI) / 3 + alpha5_), r_*cos((2 * PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin((2 * PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin((2 * PI) / 3 + alpha13_) - r_*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13_)*sin(qp[6]), r_*sin((2 * PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos((2 * PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1;
}

std::vector<uint8> MahiExoII::select_q_invert(std::vector<uint8> select_q) const {
    std::vector<uint8> indices{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    for (int i = 0; i < N_qs_; ++i) {
        indices.erase(indices.begin() + select_q[i]);
    }
    return indices;
}

//-----------------------------------------------------------------------------
// PRIVATE UTILITY FUNCTIONS
//-----------------------------------------------------------------------------

bool MahiExoII::check_goal_pos(std::vector<double> goal_pos, std::vector<double> current_pos, std::vector<char> check_dof, std::vector<double> error_tol, bool print_output) const {

    bool goal_reached = true;
    for (int i = 0; i < goal_pos.size(); ++i) {
        if (check_dof.at(i)) {
            if (std::abs(goal_pos.at(i) - current_pos.at(i)) > std::abs(error_tol.at(i))) {
                if (print_output && goal_reached) {
                    std::cout << "Joint " << std::to_string(i) << " error is " << (abs(goal_pos.at(i) - current_pos.at(i))*RAD2DEG) << std::endl;
                }
                goal_reached = false;
            }
        }
    }
    return goal_reached;
}

} // namespace mel
