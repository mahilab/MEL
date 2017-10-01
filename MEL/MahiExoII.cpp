#include "MahiExoII.h"
#include <iomanip>


namespace mel {

    namespace exo {

        //-------------------------------------------------------------------------
        // STATIC VARIABLES
        //-------------------------------------------------------------------------

        // geometric parameters
        const double MahiExoII::R_ = 0.1044956;
        const double MahiExoII::r_ = 0.05288174521;
        const double MahiExoII::a56_ = 0.0268986 - 0.0272820;
        const double MahiExoII::alpha5_ = 0.094516665054824;
        const double MahiExoII::alpha13_ = math::DEG2RAD * 5;



        //-------------------------------------------------------------------------
        // CONSTRUCTOR / DESTRUCTOR
        //-------------------------------------------------------------------------

        MahiExoII::MahiExoII(Config configuration, Params parameters) :
            Exo("mahi_exo_ii"),
            config_(configuration),
            params_(parameters)
        {

            for (int i = 0; i < N_rj_; ++i) {

                std::string num = std::to_string(i);

                // construct encoders
                core::PositionSensor* encoder = new core::Encoder("encoder_" + num,
                    params_.encoder_res_[i] / (2 * math::PI),
                    config_.encoder_[i],
                    config_.encrate_[i]);

                position_sensors_.push_back(encoder);

                // construct motors
                core::Actuator* motor = new core::Motor("motor_" + num,
                    params_.kt_[i],
                    params_.amp_gains_[i],
                    config_.command_[i],
                    config_.enable_[i],
                    core::Actuator::EnableMode::Low,
                    //params_.motor_hard_current_limits_[i]);
                    params_.motor_continuous_current_limits_[i],
                    params_.motor_peak_current_limits_[i],
                    params_.motor_i2t_times_[i]);

                actuators_.push_back(motor);

                // construct joints
                core::Joint* joint = new core::Joint("joint_" + num,
                    encoder,
                    params_.eta_[i],
                    motor,
                    params_.eta_[i],
                    std::array<double, 2>({ params_.pos_limits_min_[i] , params_.pos_limits_max_[i] }),
                    params_.vel_limits_[i],
                    params_.joint_torque_limits[i],
                    true);

                joints_.push_back(joint);
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
            if (enabled_) {
                disable();
            }
        }

        //-----------------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //-----------------------------------------------------------------------------

        void MahiExoII::disable() {

            // disable reference trajectories
            rps_init_par_ref_.stop();
            rps_par_ref_.stop();
            rps_ser_ref_.stop();
            robot_ref_.stop();
            anat_ref_.stop();

            // disable the robot
            if (enabled_) {
                // code executed by overridden function Robot::disable()
                enabled_ = false;
                util::print("Disabling Robot <" + name_ + "> ... ", false);
                for (auto it = joints_.begin(); it != joints_.end(); ++it) {
                    (*it)->disable();
                }
                util::print("Done");
            }

            
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
            default: util::print("WARNING: Invalid input argument to set_rps_control_mode(). Must be 0 or 1. No change was made.");
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

        void MahiExoII::SmoothReferenceTrajectory::start(double_vec current_pos, double current_time) {        
            if (ref_init_) {
                started_ = true;
                prev_ref_ = current_pos;
                start_time_ = current_time;
            }
            else {
                util::print("ERROR: Reference position was not initialized. Must provide reference position to start().");
            }  
        }

        void MahiExoII::SmoothReferenceTrajectory::start(double_vec ref_pos, double_vec current_pos, double current_time) {
            started_ = true;
            prev_ref_ = current_pos;
            ref_ = ref_pos;
            start_time_ = current_time;
        }

        void MahiExoII::SmoothReferenceTrajectory::set_ref(double_vec ref_pos, double current_time) {   
            if (!started_) {
                util::print("ERROR: Cannot call set_ref() before start().");
            }
            else {
                for (int i = 0; i < ref_pos.size(); ++i) {
                    prev_ref_[i] = calculate_smooth_ref(i, current_time);
                }
                ref_ = ref_pos;
                start_time_ = current_time;
            }
        }

        double MahiExoII::SmoothReferenceTrajectory::calculate_smooth_ref(int dof, double current_time) {
            if (started_) {
                if (ref_[dof] == prev_ref_[dof]) {
                    return ref_[dof];
                }
                return prev_ref_[dof] + (ref_[dof] - prev_ref_[dof]) * mel::math::saturate((current_time - start_time_) * speed_[dof] / std::abs(ref_[dof] - prev_ref_[dof]), 1.0, 0.0);
            }
            else {
                util::print("ERROR: Must give reference point first.");
                return NAN;
            }
        }

        void MahiExoII::SmoothReferenceTrajectory::stop() {
            started_ = false;
        }


        void MahiExoII::set_rps_pos_ctrl_torques(SmoothReferenceTrajectory& rps_ref, double current_time) {

            double_vec command_torques(N_qs_, 0.0);

            switch (rps_control_mode_) {
            case 0: // control impedance of parralel joints
                for (auto i = 0; i < N_qs_; ++i) {
                    if (rps_backdrive_) {
                        command_torques[i] = 0.0;
                    }
                    else {
                        double smooth_ref = rps_ref.calculate_smooth_ref(i, current_time);
                        if (std::isnan(smooth_ref)) {
                            command_torques[i] = 0.0;
                        }
                        else {
                            command_torques[i] = robot_joint_pd_controllers_[i + 2].calculate(smooth_ref, joints_[i + 2]->get_position(), 0, joints_[i + 2]->get_velocity());
                        }
                    }
                }
                set_rps_par_torques(command_torques);
                break;
            case 1: // control impedance of serial joints
                
                for (auto i = 0; i < N_qs_; ++i) {
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
                        }
                    }
                }
                set_rps_ser_torques(command_torques);
                break;

            default: util::print("WARNING: Invalid rps_control_mode_. Must be 0 or 1. Zero torques commanded.");
                for (auto i = 0; i < N_qs_; ++i) {
                    joints_[i + 2]->set_torque(0.0);
                }
            }
   
        }

        double_vec MahiExoII::set_anat_pos_ctrl_torques(SmoothReferenceTrajectory& anat_ref, double current_time) {

            double_vec command_torques(N_aj_, 0.0);

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
                    command_torques[0] = robot_joint_pd_controllers_[0].calculate(smooth_ref, joints_[0]->get_position(), 0, joints_[0]->get_velocity());
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
                    command_torques[1] = robot_joint_pd_controllers_[1].calculate(smooth_ref, joints_[1]->get_position(), 0, joints_[1]->get_velocity());
                }
            }
            

            // rps mechanism
            double_vec rps_command_torques(N_qs_, 0.0);
            switch (rps_control_mode_) {
            case 1: // control impedance of serial joints

                for (auto i = 0; i < N_qs_; ++i) {
                    if (rps_backdrive_) {
                        rps_command_torques[i] = 0.0;
                    }
                    else {
                        double smooth_ref = anat_ref.calculate_smooth_ref(i+2, current_time);
                        if (std::isnan(smooth_ref)) {
                            rps_command_torques[i] = 0.0;
                        }
                        else {
                            rps_command_torques[i] = anatomical_joint_pd_controllers_[i + 2].calculate(smooth_ref, get_anatomical_joint_position(i + 2), 0, get_anatomical_joint_velocity(i + 2));
                        }
                    }
                }
                break;

            default: util::print("WARNING: Invalid rps_control_mode_. Must be 1. Zero torques commanded.");
                for (auto i = 0; i < N_qs_; ++i) {
                    rps_command_torques = double_vec(N_qs_, 0.0);
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
            q_par_ << joints_[2]->get_position(), joints_[3]->get_position(), joints_[4]->get_position();
            q_par_dot_ << joints_[2]->get_velocity(), joints_[3]->get_velocity(), joints_[4]->get_velocity();

            // run forward kinematics solver to update q_ser (q serial) and qp_ (q prime), which contains all 12 RPS positions
            forward_rps_kinematics_velocity(q_par_, q_ser_, qp_, rho_fk_, jac_fk_, q_par_dot_, q_ser_dot_, qp_dot_);

            // get positions from first two anatomical joints, which have encoders
            anatomical_joint_positions_[0] = joints_[0]->get_position(); // elbow flexion/extension
            anatomical_joint_positions_[1] = joints_[1]->get_position(); // forearm pronation/supination

            // get positions from forward kinematics solver for three wrist anatomical joints 
            anatomical_joint_positions_[2] = q_ser_[0]; // wrist flexion/extension
            anatomical_joint_positions_[3] = q_ser_[1]; // wrist radial/ulnar deviation
            anatomical_joint_positions_[4] = q_ser_[2]; // arm translation

            // get velocities from first two anatomical joints, which have encoders
            anatomical_joint_velocities_[0] = joints_[0]->get_velocity(); // elbow flexion/extension
            anatomical_joint_velocities_[1] = joints_[1]->get_velocity(); // forearm pronation/supination

            // get velocities from forward kinematics solver for three wrist anatomical joints 
            anatomical_joint_velocities_[2] = q_ser_dot_[0]; // wrist flexion/extension
            anatomical_joint_velocities_[3] = q_ser_dot_[1]; // wrist radial/ulnar deviation
            anatomical_joint_velocities_[4] = q_ser_dot_[2]; // arm translation
        }

        double_vec MahiExoII::get_wrist_parallel_positions() const {
            return math::eigenv2stdv(q_par_);
        }

        double_vec MahiExoII::get_wrist_serial_positions() const {
            return math::eigenv2stdv(q_ser_);
        }

        void MahiExoII::set_anatomical_joint_torques(double_vec new_torques, int error_code) {

            // set torques for first two anatomical joints, which have actuators
            joints_[0]->set_torque(new_torques[0]);
            joints_[1]->set_torque(new_torques[1]);

            // calculate the spectral norm of the transformation matrix
            Eigen::EigenSolver<Eigen::Matrix3d> eigensolver(jac_fk_.transpose() * jac_fk_, false);
            if (eigensolver.info() != Eigen::Success) {
                joints_[2]->set_torque(0.0);
                joints_[3]->set_torque(0.0);
                joints_[4]->set_torque(0.0);
                error_code_ = -1;
            }
            Eigen::EigenSolver<Eigen::Matrix3d>::EigenvalueType lambda = eigensolver.eigenvalues();
            double_vec lambda_abs;
            for (int i = 0; i < N_qs_; ++i) {
                lambda_abs.push_back(std::abs(lambda(i)));
            }
            std::vector<double>::iterator lambda_max;
            lambda_max = std::max_element(lambda_abs.begin(), lambda_abs.end());
            double spec_norm = std::sqrt(*lambda_max);
            //mel::print(spec_norm);

            // kill robot if norm too large
            if (spec_norm > 100) {
                error_code_ = -3;
            }

            if (spec_norm_prev_ != 0) {
                if (std::abs(spec_norm - spec_norm_prev_) > 100) {
                    std::cout << std::setprecision(12);
                    std::cout << q_par_prev_.transpose() << std::endl;
                    std::cout << q_par_.transpose() << std::endl;
                    std::cout << qp_.transpose() << std::endl;
                    std::cout << jac_fk_ << std::endl;
                    //mel::print(lambda_abs);
                    error_code_ = -2;
                }
            }

            // debugging
            spec_norm_prev_ = spec_norm;
            q_par_prev_ = q_par_;
            //mel::print(eigensolver.eigenvalues());

            // set torques for two wrist anatomical joints and arm translation
            Eigen::VectorXd par_torques = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd ser_torques = Eigen::VectorXd::Zero(N_qs_);
            ser_torques(0) = new_torques[2];
            ser_torques(1) = new_torques[3];
            ser_torques(2) = new_torques[4];
            par_torques = jac_fk_.transpose()*ser_torques;

            joints_[2]->set_torque(par_torques(0));
            joints_[3]->set_torque(par_torques(1));
            joints_[4]->set_torque(par_torques(2));
            /*joints_[2]->set_torque(0.0);
            joints_[3]->set_torque(0.0);
            joints_[4]->set_torque(0.0);*/
            //std::cout << jac_fk_.transpose() << std::endl;
            //std::cout << par_torques.transpose() << std::endl;

        }


        void MahiExoII::set_rps_par_torques(double_vec& tau_par) {
            for (int i = 0; i < N_qs_; ++i) {
                joints_[i + 2]->set_torque(tau_par[i]);
            }
        }

        void MahiExoII::set_rps_ser_torques(double_vec& tau_ser) {
            Eigen::VectorXd tau_ser_eig = math::stdv2eigenv(tau_ser);
            Eigen::VectorXd tau_par = Eigen::VectorXd::Zero(N_qs_);
            tau_par = jac_fk_.transpose() * tau_ser_eig;
            for (int i = 0; i < N_qs_; ++i) {
                joints_[i + 2]->set_torque(tau_par[i]);
            }
        }


        void MahiExoII::forward_rps_kinematics(double_vec& q_par_in, double_vec& q_ser_out) const {
            double_vec qp;
            forward_rps_kinematics(q_par_in, q_ser_out, qp);
        }

        void MahiExoII::forward_rps_kinematics(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out) const {

            Eigen::VectorXd q_par_in_eig = math::stdv2eigenv(q_par_in);
            Eigen::VectorXd q_ser_out_eig = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);

            forward_rps_kinematics(q_par_in_eig, q_ser_out_eig, qp_out_eig);

            q_ser_out = math::eigenv2stdv(q_ser_out_eig);
            qp_out = math::eigenv2stdv(qp_out_eig);

        }

        void MahiExoII::forward_rps_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out) const {
            double_vec qp, qp_dot;
            forward_rps_kinematics_velocity(q_par_in, q_ser_out, qp, q_par_dot_in, q_ser_dot_out, qp_dot);
        }


        void MahiExoII::forward_rps_kinematics_velocity(double_vec& q_par_in, double_vec& q_ser_out, double_vec& qp_out, double_vec& q_par_dot_in, double_vec& q_ser_dot_out, double_vec& qp_dot_out) const {

            Eigen::VectorXd q_par_in_eig = math::stdv2eigenv(q_par_in);
            Eigen::VectorXd q_ser_out_eig = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);
            Eigen::VectorXd q_par_dot_in_eig = math::stdv2eigenv(q_par_dot_in);
            Eigen::VectorXd q_ser_dot_out_eig = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd qp_dot_out_eig = Eigen::VectorXd::Zero(N_qs_);

            forward_rps_kinematics_velocity(q_par_in_eig, q_ser_out_eig, qp_out_eig, q_par_dot_in_eig, q_ser_dot_out_eig, qp_dot_out_eig);

            q_ser_out = math::eigenv2stdv(q_ser_out_eig);
            qp_out = math::eigenv2stdv(qp_out_eig);
            q_ser_dot_out = math::eigenv2stdv(q_ser_dot_out_eig);
            qp_dot_out = math::eigenv2stdv(qp_dot_out_eig);

        }

        void MahiExoII::inverse_rps_kinematics(double_vec& q_ser_in, double_vec& q_par_out) const {
            double_vec qp;
            inverse_rps_kinematics(q_ser_in, q_par_out, qp);
        }

        void MahiExoII::inverse_rps_kinematics(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out) const {

            Eigen::VectorXd q_ser_in_eig = math::stdv2eigenv(q_ser_in);
            Eigen::VectorXd q_par_out_eig = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);

            inverse_rps_kinematics(q_ser_in_eig, q_par_out_eig, qp_out_eig);

            q_par_out = math::eigenv2stdv(q_par_out_eig);
            qp_out = math::eigenv2stdv(qp_out_eig);

        }

        void MahiExoII::inverse_rps_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out) const {
            double_vec qp, qp_dot;
            inverse_rps_kinematics_velocity(q_ser_in, q_par_out, qp, q_ser_dot_in, q_par_dot_out, qp_dot);
        }

        void MahiExoII::inverse_rps_kinematics_velocity(double_vec& q_ser_in, double_vec& q_par_out, double_vec& qp_out, double_vec& q_ser_dot_in, double_vec& q_par_dot_out, double_vec& qp_dot_out) const {

            Eigen::VectorXd q_ser_in_eig = math::stdv2eigenv(q_ser_in);
            Eigen::VectorXd q_par_out_eig = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd qp_out_eig = Eigen::VectorXd::Zero(N_qp_);
            Eigen::VectorXd q_ser_dot_in_eig = math::stdv2eigenv(q_ser_dot_in);
            Eigen::VectorXd q_par_dot_out_eig = Eigen::VectorXd::Zero(N_qs_);
            Eigen::VectorXd qp_dot_out_eig = Eigen::VectorXd::Zero(N_qp_);

            inverse_rps_kinematics_velocity(q_ser_in_eig, q_par_out_eig, qp_out_eig, q_ser_dot_in_eig, q_par_dot_out_eig, qp_dot_out_eig);

            q_par_out = math::eigenv2stdv(q_par_out_eig);
            qp_out = math::eigenv2stdv(qp_out_eig);
            q_par_dot_out = math::eigenv2stdv(q_par_dot_out_eig);
            qp_dot_out = math::eigenv2stdv(qp_dot_out_eig);

        }

        
        //-----------------------------------------------------------------------------
        // PUBLIC UTILITY FUNCTIONS
        //-----------------------------------------------------------------------------

        bool MahiExoII::check_rps_init(bool print_output) const {
            double_vec rps_pos_tol_vec(N_qs_, rps_init_err_tol_);
            return check_goal_pos(rps_init_pos_, get_wrist_parallel_positions(), { 1,1,1 }, rps_pos_tol_vec, print_output);
        }

        bool MahiExoII::check_goal_rps_par_pos(double_vec goal_rps_par_pos, char_vec check_dof, bool print_output) const {
            return check_goal_pos(goal_rps_par_pos, get_wrist_parallel_positions(), check_dof, rps_par_goal_err_tol_, print_output );
        }

        bool MahiExoII::check_goal_rps_ser_pos(double_vec goal_rps_ser_pos, char_vec check_dof, bool print_output) const {
            return check_goal_pos(goal_rps_ser_pos, get_wrist_serial_positions(), check_dof, rps_ser_goal_err_tol_, print_output);
        }

        bool MahiExoII::check_goal_anat_pos(double_vec goal_anat_pos, char_vec check_dof, bool print_output) const {
            return check_goal_pos(goal_anat_pos, get_anatomical_joint_positions(), check_dof, anat_goal_err_tol_, print_output);
        }

        //-----------------------------------------------------------------------------
        // PRIVATE KINEMATICS FUNCTIONS
        //-----------------------------------------------------------------------------


        void MahiExoII::solve_static_rps_torques(uint8_vec select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s) const {
            Eigen::MatrixXd rho = Eigen::MatrixXd::Zero(N_qp_ - N_qs_, N_qs_);
            generate_rho(select_q, qp, rho);
            tau_s = rho.transpose() * tau_b;
        }

        void MahiExoII::solve_static_rps_torques(uint8_vec select_q, const Eigen::VectorXd& tau_b, const Eigen::VectorXd& qp, Eigen::VectorXd& tau_s, Eigen::VectorXd& tau_p) const {
            solve_static_rps_torques(select_q, tau_b, qp, tau_s);
            for (int i = 0; i < N_qs_; ++i) {
                tau_p[select_q[i]] = tau_s[i];
            }
            uint8_vec indices = select_q_invert(select_q);
            for (int i = 0; i < N_qp_ - N_qs_; ++i) {
                tau_p[indices[i]] = tau_b[i];
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
            uint8_vec indices = select_q_invert(select_q_ser_);
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
                qp_dot_out[select_q_par_[i]] = q_par_dot_out[i];
            }
            uint8_vec indices = select_q_invert(select_q_par_);
            for (int i = 0; i < N_qp_ - N_qs_; ++i) {
                qp_dot_out[indices[i]] = qb_dot[i];
            }
        }

        void MahiExoII::solve_rps_kinematics(uint8_vec select_q, const Eigen::VectorXd& qs, Eigen::VectorXd& qp, Eigen::MatrixXd& rho, Eigen::MatrixXd& rho_s, uint32 max_it, double tol) const {

            // build selection matrix
            Eigen::MatrixXd A = Eigen::MatrixXd::Zero(N_qs_, N_qp_);
            for (int i = 0; i < N_qs_; ++i) {
                A(i, select_q[i]) = 1;
            }

            // initialize variable containing solution
            qp << math::PI / 4, math::PI / 4, math::PI / 4, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0.0923, 0, 0;

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
            uint8_vec indices = select_q_invert(select_q);
            for (int i = 0; i < N_qp_ - N_qs_; ++i) {
                rho.row(i) = rho_s.row(indices[i]);
            }
        }

        void MahiExoII::generate_rho(uint8_vec select_q, const Eigen::VectorXd& qp, Eigen::MatrixXd& rho) const {
            
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
            uint8_vec indices = select_q_invert(select_q);
            for (int i = 0; i < N_qp_ - N_qs_; ++i) {
                rho.row(i) = rho_s.row(indices[i]);
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
                qp[4] * sin(qp[1]) - qp[9] - r_*cos(alpha13_ - (2 * math::PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin(alpha13_ - (2 * math::PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
                R_*cos(alpha5_ - (2 * math::PI) / 3) - qp[10] - a56_*sin(alpha5_ - (2 * math::PI) / 3) - qp[4] * cos(alpha5_ - (2 * math::PI) / 3)*cos(qp[1]) - r_*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * math::PI) / 3) + r_*cos(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * math::PI) / 3),
                a56_*cos(alpha5_ - (2 * math::PI) / 3) - qp[11] + R_*sin(alpha5_ - (2 * math::PI) / 3) - qp[4] * cos(qp[1])*sin(alpha5_ - (2 * math::PI) / 3) - r_*cos(alpha13_ - (2 * math::PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_ - (2 * math::PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
                qp[5] * sin(qp[2]) - qp[9] - r_*cos((2 * math::PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*sin((2 * math::PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
                R_*cos((2 * math::PI) / 3 + alpha5_) - qp[10] - a56_*sin((2 * math::PI) / 3 + alpha5_) - qp[5] * cos((2 * math::PI) / 3 + alpha5_)*cos(qp[2]) - r_*cos(qp[7])*cos(qp[8])*cos((2 * math::PI) / 3 + alpha13_) + r_*cos(qp[7])*sin(qp[8])*sin((2 * math::PI) / 3 + alpha13_),
                a56_*cos((2 * math::PI) / 3 + alpha5_) - qp[11] + R_*sin((2 * math::PI) / 3 + alpha5_) - qp[5] * cos(qp[2])*sin((2 * math::PI) / 3 + alpha5_) - r_*cos((2 * math::PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin((2 * math::PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8]));
        }

        void MahiExoII::phi_d_qp_update(const Eigen::VectorXd& qp, Eigen::MatrixXd& phi_d_qp) const {

            phi_d_qp << qp[3] * cos(qp[0]), 0, 0, sin(qp[0]), 0, 0, -r_*cos(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(alpha13_)*cos(qp[7])*cos(qp[8]) - r_*cos(qp[6])*cos(qp[7])*sin(alpha13_)*sin(qp[8]), r_*sin(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                qp[3] * cos(alpha5_)*sin(qp[0]), 0, 0, -cos(alpha5_)*cos(qp[0]), 0, 0, 0, r_*cos(alpha13_)*cos(qp[8])*sin(qp[7]) - r_*sin(alpha13_)*sin(qp[7])*sin(qp[8]), r_*cos(alpha13_)*cos(qp[7])*sin(qp[8]) + r_*cos(qp[7])*cos(qp[8])*sin(alpha13_), 0, -1, 0,
                qp[3] * sin(alpha5_)*sin(qp[0]), 0, 0, -sin(alpha5_)*cos(qp[0]), 0, 0, r_*cos(alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin(alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(alpha13_)*sin(qp[8]) - r_*cos(alpha13_)*cos(qp[7])*cos(qp[8])*sin(qp[6]), r_*sin(alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos(alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                0, qp[4] * cos(qp[1]), 0, 0, sin(qp[1]), 0, -r_*cos(alpha13_ - (2 * math::PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin(alpha13_ - (2 * math::PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * math::PI) / 3) - r_*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * math::PI) / 3), r_*sin(alpha13_ - (2 * math::PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos(alpha13_ - (2 * math::PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                0, qp[4] * cos(alpha5_ - (2 * math::PI) / 3)*sin(qp[1]), 0, 0, -cos(alpha5_ - (2 * math::PI) / 3)*cos(qp[1]), 0, 0, r_*cos(qp[8])*cos(alpha13_ - (2 * math::PI) / 3)*sin(qp[7]) - r_*sin(qp[7])*sin(qp[8])*sin(alpha13_ - (2 * math::PI) / 3), r_*cos(qp[7])*cos(qp[8])*sin(alpha13_ - (2 * math::PI) / 3) + r_*cos(qp[7])*cos(alpha13_ - (2 * math::PI) / 3)*sin(qp[8]), 0, -1, 0,
                0, qp[4] * sin(alpha5_ - (2 * math::PI) / 3)*sin(qp[1]), 0, 0, -cos(qp[1])*sin(alpha5_ - (2 * math::PI) / 3), 0, r_*cos(alpha13_ - (2 * math::PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin(alpha13_ - (2 * math::PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin(alpha13_ - (2 * math::PI) / 3) - r_*cos(qp[7])*cos(qp[8])*cos(alpha13_ - (2 * math::PI) / 3)*sin(qp[6]), r_*sin(alpha13_ - (2 * math::PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos(alpha13_ - (2 * math::PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                0, 0, qp[5] * cos(qp[2]), 0, 0, sin(qp[2]), -r_*cos((2 * math::PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*sin((2 * math::PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos((2 * math::PI) / 3 + alpha13_) - r_*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin((2 * math::PI) / 3 + alpha13_), r_*sin((2 * math::PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r_*cos((2 * math::PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                0, 0, qp[5] * cos((2 * math::PI) / 3 + alpha5_)*sin(qp[2]), 0, 0, -cos((2 * math::PI) / 3 + alpha5_)*cos(qp[2]), 0, r_*cos(qp[8])*cos((2 * math::PI) / 3 + alpha13_)*sin(qp[7]) - r_*sin(qp[7])*sin(qp[8])*sin((2 * math::PI) / 3 + alpha13_), r_*cos(qp[7])*cos(qp[8])*sin((2 * math::PI) / 3 + alpha13_) + r_*cos(qp[7])*cos((2 * math::PI) / 3 + alpha13_)*sin(qp[8]), 0, -1, 0,
                0, 0, qp[5] * sin((2 * math::PI) / 3 + alpha5_)*sin(qp[2]), 0, 0, -cos(qp[2])*sin((2 * math::PI) / 3 + alpha5_), r_*cos((2 * math::PI) / 3 + alpha13_)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r_*sin((2 * math::PI) / 3 + alpha13_)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r_*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin((2 * math::PI) / 3 + alpha13_) - r_*cos(qp[7])*cos(qp[8])*cos((2 * math::PI) / 3 + alpha13_)*sin(qp[6]), r_*sin((2 * math::PI) / 3 + alpha13_)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r_*cos((2 * math::PI) / 3 + alpha13_)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1;
        }

        uint8_vec MahiExoII::select_q_invert(uint8_vec select_q) const {
            uint8_vec indices{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
            for (int i = 0; i < N_qs_; ++i) {
                indices.erase(indices.begin() + select_q[i]);
            }
            return indices;
        }

        //-----------------------------------------------------------------------------
        // PRIVATE UTILITY FUNCTIONS
        //-----------------------------------------------------------------------------

        bool MahiExoII::check_goal_pos(double_vec goal_pos, double_vec current_pos, char_vec check_dof, double_vec error_tol, bool print_output) const {

            bool goal_reached = true;
            for (int i = 0; i < goal_pos.size(); ++i) {
                if (check_dof[i]) {
                    if (std::abs(goal_pos[i] - current_pos[i]) > std::abs(error_tol[i])) {
                        if (print_output && goal_reached) {
                            std::cout << "Joint " << std::to_string(i) << " error is " << (abs(goal_pos[i] - current_pos[i])*math::RAD2DEG) << std::endl;
                        }
                        goal_reached = false;
                    }
                }
            }
            return goal_reached;
        }

    }
}
