#include "FesExperiment.h"
#include "Input.h"
#include "mahiexoii_util.h"


FesExperiment::FesExperiment(mel::Clock& clock, mel::Daq* q8_emg, mel::MahiExoII& meii, int subject_number, int trial) :
    StateMachine(9),
    clock_(clock),
    q8_emg_(q8_emg),
    meii_(meii),
    SUBJECT_NUMBER_(subject_number),
    TRIAL_(trial),
    CONDITION_(cond_mat_[subject_number-1][trial])
{

    if (subject_number < 1 || subject_number > 10) {
        mel::print("ERROR: Subject number out of range (1-10)");
    }
    if (trial < 0 || subject_number > 27) {
        mel::print("ERROR: Trial number out of range (0-27)");
    }

    // create subject folder
    if (subject_number < 10) {
        DIRECTORY_ = "FES_S0" + std::to_string(subject_number);
    }
    else {
        DIRECTORY_ = "FES_S" + std::to_string(subject_number);
    }

    // Add columns to logger
    log_.add_col("Time [s]").add_col("State")
        .add_col("Reference Elbow Position [deg]").add_col("Anatomical Elbow Position [deg]")
        .add_col("MEII EFE Position [deg]").add_col("MEII EFE Velocity [deg/s]").add_col("MEII EFE Commanded Torque [Nm]")
        .add_col("MEII FPS Position [deg]").add_col("MEII FPS Velocity [deg/s]").add_col("MEII FPS Commanded Torque [Nm]")
        .add_col("MEII RPS1 Position [m]").add_col("MEII RPS1 Velocity [m/s]").add_col("MEII RPS1 Commanded Force [N]")
        .add_col("MEII RPS2 Position [m]").add_col("MEII RPS2 Velocity [m/s]").add_col("MEII RPS2 Commanded Force [N]")
        .add_col("MEII RPS3 Position [m]").add_col("MEII RPS3 Velocity [m/s]").add_col("MEII RPS3 Commanded Force [N]");
}

void FesExperiment::log_row() {
    std::vector<double> row;
    row.push_back(clock_.time());
    row.push_back(get_current_state());
    row.push_back(elbow_ref_pos_deg_);
    row.push_back(elbow_pos_deg_);
    row.push_back(meii_.joints_[0]->get_position() * mel::RAD2DEG);
    row.push_back(meii_.joints_[0]->get_velocity() * mel::RAD2DEG);
    row.push_back(static_cast<mel::Motor*>(meii_.actuators_[0])->get_torque_command());
    row.push_back(meii_.joints_[1]->get_position() * mel::RAD2DEG);
    row.push_back(meii_.joints_[1]->get_velocity() * mel::RAD2DEG);
    row.push_back(static_cast<mel::Motor*>(meii_.actuators_[1])->get_torque_command());
    row.push_back(meii_.joints_[2]->get_position() * mel::RAD2DEG);
    row.push_back(meii_.joints_[2]->get_velocity() * mel::RAD2DEG);
    row.push_back(static_cast<mel::Motor*>(meii_.actuators_[2])->get_torque_command());
    row.push_back(meii_.joints_[3]->get_position() * mel::RAD2DEG);
    row.push_back(meii_.joints_[3]->get_velocity() * mel::RAD2DEG);
    row.push_back(static_cast<mel::Motor*>(meii_.actuators_[3])->get_torque_command());
    row.push_back(meii_.joints_[4]->get_position() * mel::RAD2DEG);
    row.push_back(meii_.joints_[4]->get_velocity() * mel::RAD2DEG);
    row.push_back(static_cast<mel::Motor*>(meii_.actuators_[4])->get_torque_command());
    log_.add_row(row);
}

void FesExperiment::wait_for_input() {
     mel::Input::wait_for_key_press(mel::Input::Key::Space);
}

bool FesExperiment::check_stop() {
    return mel::Input::is_key_pressed(mel::Input::Escape) || (mel::Input::is_key_pressed(mel::Input::LControl) && mel::Input::is_key_pressed(mel::Input::C));
}

//-----------------------------------------------------------------------------
// "INITIALIZATION" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_init(const mel::NoEventData* data) {

    // initialize UDP
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
    if ((s_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //setup address structure
    memset((char *)&si_other_, 0, sizeof(si_other_));
    si_other_.sin_family = AF_INET;
    si_other_.sin_port = htons(PORT);
    si_other_.sin_addr.S_un.S_addr = inet_addr(SERVER);

    // enable MEII EMG DAQ
    mel::print("\nPress Enter to enable MEII EMG Daq <" + q8_emg_->name_ + ">.");
    mel::Input::wait_for_key_press(mel::Input::Key::Return);
    q8_emg_->enable();
    if (!q8_emg_->is_enabled()) {
        event(ST_STOP);
        return;
    }

    // check DAQ behavior for safety
    q8_emg_->read_all();
    meii_.update_kinematics();
    if (meii_.check_all_joint_limits()) {
        event(ST_STOP);
        return;
    }
    if (!check_digital_loopback(0, 7)) {
        event(ST_STOP);
        return;
    }

    // enable MEII
    mel::print("\nPress Enter to enable MEII.");
    mel::Input::wait_for_key_press(mel::Input::Key::Return);
    meii_.enable();
    if (!meii_.is_enabled()) {
        event(ST_STOP);
        return;
    }

    // confirm start of experiment
    std::cout << "Press Enter to start the controller" << std::endl;
    mel::Input::wait_for_key_press(mel::Input::Key::Return);
    q8_emg_->start_watchdog(0.1);
    std::cout << "Starting the controller ... " << std::endl;

    // check for stop input
    stop_ = check_stop();

    // start the clock
    clock_.start();

    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else {
        event(ST_TRANSPARENT);
    }

}


//-----------------------------------------------------------------------------
// "TRANSPARENT" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_transparent(const mel::NoEventData* data) {
    mel::print("Robot Transparent");

    if (CONDITION_ == 0 && initial_position_reached_) {
        clock_.start();
        init_transparent_time_ = flexion_trajectory_time_ + hold_flexed_time_ + extension_trajectory_time_ + hold_extended_time_;
    }

    // initialize event variables
    st_enter_time_ = clock_.time();
    init_transparent_time_reached_ = false;

    // enter the control loop
    while (!init_transparent_time_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute anatomical elbow position
        elbow_pos_deg_ = compute_elbow_anatomical_position(meii_.get_anatomical_joint_position(0)) * mel::RAD2DEG;

        if (CONDITION_ == 0 && initial_position_reached_) {

            // send elbow joint angle and time through UDP
            send_udp_packet(elbow_pos_deg_); 

            // update elbow reference position
            if (clock_.time() <= flexion_trajectory_time_) {
                elbow_ref_pos_deg_ = compute_elbow_anatomical_position(compute_elbow_flexion_trajectory(st_enter_time_, clock_.time())) * mel::RAD2DEG;
            }
            else if ((clock_.time() > flexion_trajectory_time_) && (clock_.time() <= (flexion_trajectory_time_ + hold_flexed_time_)) ) {
                elbow_ref_pos_deg_ = compute_elbow_anatomical_position(elbow_flexed_pos_) * mel::RAD2DEG;
            }
            else if (clock_.time() > flexion_trajectory_time_ + hold_flexed_time_) {
                elbow_ref_pos_deg_ = compute_elbow_anatomical_position(compute_elbow_extension_trajectory(st_enter_time_ + flexion_trajectory_time_ + hold_flexed_time_, clock_.time())) * mel::RAD2DEG;
            }
        }

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // set zero torques
        commanded_torques_ = { 0,0,0,0,0 };
        meii_.set_anatomical_joint_torques(commanded_torques_);

        // write to daq
        q8_emg_->write_all();

        // check for init transparent time reached
        if (CONDITION_ != 3) {
            init_transparent_time_reached_ = check_wait_time_reached(init_transparent_time_, st_enter_time_, clock_.time());
        }
        else {
            mel::print(elbow_pos_deg_);
        }

        // log data
        log_row();

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else if (init_transparent_time_reached_) {
        init_transparent_time_reached_ = false; // reset flag
        if (CONDITION_ == 0) {
            if (!initial_position_reached_) {
                event(ST_TO_EXTENDED);
            }
            else {
                event(ST_FINISH);
            }
        }
        else if (CONDITION_ == 1 || CONDITION_ == 2) {
            event(ST_TO_EXTENDED);
        }
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}


//-----------------------------------------------------------------------------
// "GO TO EXTENDED" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_to_extended(const mel::NoEventData* data) {
    mel::print("Go to Extended Position");

    // initialize event variables
    st_enter_time_ = clock_.time();
    target_reached_ = false;

    // get current position and time to initialize trajectory
    q8_emg_->reload_watchdog();
    q8_emg_->read_all();
    meii_.update_kinematics();
    init_pos_ = meii_.get_anatomical_joint_positions();

    // set goal position for start of trajectory
    goal_pos_ = extended_pos_;

    // set joints to be checked for target reached
    target_check_joint_ = { 1, 0, 0, 0, 0 };

    // enter the control loop
    while (!target_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute anatomical elbow position
        elbow_pos_deg_ = compute_elbow_anatomical_position(meii_.get_anatomical_joint_position(0)) * mel::RAD2DEG;

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // compute pd torques
        for (auto i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], st_enter_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));    
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // set new torques
        commanded_torques_ = new_torques_;
        meii_.set_anatomical_joint_torques(commanded_torques_);

        // write to daq
        q8_emg_->write_all();

        // check for target reached
        target_reached_ = check_target_reached(goal_pos_, meii_.get_anatomical_joint_positions(), target_check_joint_);

        // log data
        log_row();

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to next state
    if (stop_) {
        event(ST_STOP); // stop if user provided input
    }
    else if (target_reached_) {
        target_reached_ = false; // reset flag
        initial_position_reached_ = true;
        event(ST_HOLD_EXTENDED);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "HOLD AT EXTENDED" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_hold_extended(const mel::NoEventData* data) {
    mel::print("Hold at Extended Position");

    // initialize event variables
    st_enter_time_ = clock_.time();
    hold_extended_time_reached_ = false;

    // initialize moving set point
    init_pos_ = extended_pos_;
    goal_pos_ = extended_pos_;

    // enter the control loop
    while (!hold_extended_time_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute anatomical elbow position
        elbow_pos_deg_ = compute_elbow_anatomical_position(meii_.get_anatomical_joint_position(0)) * mel::RAD2DEG;

        if (flexion_trajectory_finished_ && extension_trajectory_finished_) {

            // send elbow joint angle and time through UDP
            send_udp_packet(elbow_pos_deg_);
        }

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // compute pd torques
        for (int i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], st_enter_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // set new torques
        commanded_torques_ = new_torques_;
        meii_.set_anatomical_joint_torques(commanded_torques_);

        // write to daq
        q8_emg_->write_all();

        // check for hold time reached
        hold_extended_time_reached_ = check_wait_time_reached(hold_extended_time_, st_enter_time_, clock_.time());

        // log data
        log_row();

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else if (hold_extended_time_reached_) {
        hold_extended_time_reached_ = false; // reset flag
        if (CONDITION_ == 0) {
            event(ST_TRANSPARENT);
        }
        else if (CONDITION_ == 1 || CONDITION_ == 2) {
            if (flexion_trajectory_finished_ && extension_trajectory_finished_) {
                event(ST_FINISH);
            }
            else {
                event(ST_FLEXION_TRAJECTORY);
            }
        }
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "FLEXION TRAJECTORY" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_flexion_trajectory(const mel::NoEventData* data) {
    mel::print("Flexion Trajectory");

    // restart the clock
    clock_.start();

    // initialize event variables
    st_enter_time_ = clock_.time();

    // initialize reference position
    x_ref_ = extended_pos_;

    // enter the control loop
    while (!flexion_trajectory_finished_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute anatomical elbow position
        elbow_pos_deg_ = compute_elbow_anatomical_position(meii_.get_anatomical_joint_position(0)) * mel::RAD2DEG;

        // send elbow joint angle and time through UDP
        send_udp_packet(elbow_pos_deg_);

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // update elbow reference position
        x_ref_[0] = compute_elbow_flexion_trajectory(st_enter_time_, clock_.time());
        elbow_ref_pos_deg_ = compute_elbow_anatomical_position(x_ref_[0]) * mel::RAD2DEG;

        // compute pd torques
        for (int i = 0; i < 5; ++i) {
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // set new torques
        commanded_torques_ = new_torques_;
        meii_.set_anatomical_joint_torques(commanded_torques_);

        // write to daq
        q8_emg_->write_all();

        // check for trajectory finished
        flexion_trajectory_finished_ = check_wait_time_reached(flexion_trajectory_time_, st_enter_time_, clock_.time());

        // log data
        log_row();

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else if (flexion_trajectory_finished_) {
        event(ST_HOLD_FLEXED);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "HOLD AT FLEXED" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_hold_flexed(const mel::NoEventData* data) {
    mel::print("Hold at Flexed Position");

    // initialize event variables
    st_enter_time_ = clock_.time();
    hold_flexed_time_reached_ = false;

    // initialize moving set point
    init_pos_ = flexed_pos_;
    goal_pos_ = flexed_pos_;

    // enter the control loop
    while (!hold_flexed_time_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute anatomical elbow position
        elbow_pos_deg_ = compute_elbow_anatomical_position(meii_.get_anatomical_joint_position(0)) * mel::RAD2DEG;

        // send elbow joint angle and time through UDP
        send_udp_packet(elbow_pos_deg_);

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // compute pd torques
        for (int i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], st_enter_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // set new torques
        commanded_torques_ = new_torques_;
        meii_.set_anatomical_joint_torques(commanded_torques_);

        // write to daq
        q8_emg_->write_all();

        // check for hold time reached
        hold_flexed_time_reached_ = check_wait_time_reached(hold_flexed_time_, st_enter_time_, clock_.time());

        // log data
        log_row();

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else if (hold_flexed_time_reached_) {
        hold_flexed_time_reached_ = false; // reset flag
        event(ST_EXTENSION_TRAJECTORY);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "EXTENSION TRAJECTORY" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_extension_trajectory(const mel::NoEventData* data) {
    mel::print("Extension Trajectory");

    // initialize event variables
    st_enter_time_ = clock_.time();

    // initialize reference position
    x_ref_ = flexed_pos_;

    // enter the control loop
    while (!extension_trajectory_finished_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute anatomical elbow position
        elbow_pos_deg_ = compute_elbow_anatomical_position(meii_.get_anatomical_joint_position(0)) * mel::RAD2DEG;

        // send elbow joint angle and time through UDP
        send_udp_packet(elbow_pos_deg_);

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // update elbow reference position
        x_ref_[0] = compute_elbow_extension_trajectory(st_enter_time_, clock_.time());
        elbow_ref_pos_deg_ = compute_elbow_anatomical_position(x_ref_[0]) * mel::RAD2DEG;

        // compute pd torques
        for (int i = 0; i < 5; ++i) {
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // set new torques
        commanded_torques_ = new_torques_;
        meii_.set_anatomical_joint_torques(commanded_torques_);

        // write to daq
        q8_emg_->write_all();

        // check for trajectory finished
        extension_trajectory_finished_ = check_wait_time_reached(extension_trajectory_time_, st_enter_time_, clock_.time());

        // log data
        log_row();

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else if (extension_trajectory_finished_) {
        event(ST_HOLD_EXTENDED);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "FINISH Experiment" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_finish(const mel::NoEventData* data) {
    mel::print("Finish Experiment and Stop Robot");

    if (meii_.is_enabled()) {
        meii_.disable();
    }
    if (q8_emg_->is_enabled()) {
        q8_emg_->disable();
    }

    std::string filename;
    if (SUBJECT_NUMBER_ < 10) {
        filename = "fes_exp_data_S0" + std::to_string(SUBJECT_NUMBER_);
    }
    else {
        filename = "fes_exp_data_S" + std::to_string(SUBJECT_NUMBER_);
    }
    if (TRIAL_ < 10) {
        filename = filename + "_T0" + std::to_string(TRIAL_) + "_C" + std::to_string(CONDITION_);
    }
    else {
        filename = filename + "_T" + std::to_string(TRIAL_) + "_C" + std::to_string(CONDITION_);
    }



    log_.save_data(filename, DIRECTORY_, true);

    event(ST_STOP);

}

//-----------------------------------------------------------------------------
// "STOP" STATE FUNCTION
//-----------------------------------------------------------------------------

void FesExperiment::sf_stop(const mel::NoEventData* data) {
    std::cout << "Stop Robot" << std::endl;
    if (meii_.is_enabled()) {
        meii_.disable();
    }
    if (q8_emg_->is_enabled()) {
        q8_emg_->disable();
    }
}

//-----------------------------------------------------------------------------
// UTILITY FUNCTIONS
//-----------------------------------------------------------------------------

bool FesExperiment::check_target_reached(mel::double_vec goal_pos, mel::double_vec current_pos, mel::char_vec target_check_joint, bool print_output) {

    bool target_reached = true;
    for (int i = 0; i < 5; ++i) {
        if (target_check_joint[i]) {
            if (std::abs(goal_pos[i] - current_pos[i]) > std::abs(target_tol_[i])) {
                if (print_output && target_reached) {
                    std::cout << "Joint " << std::to_string(i) << " error is " << (abs(goal_pos[i] - current_pos[i])*mel::RAD2DEG) << std::endl;
                }
                target_reached = false;
            }
        }
    }
    return target_reached;
}

bool FesExperiment::check_wait_time_reached(double wait_time, double init_time, double current_time) {
    return (current_time - init_time) >= wait_time;
}



double FesExperiment::compute_elbow_anatomical_position(double robot_elbow_position) {
    return robot_elbow_position - elbow_extended_pos_;
}

double FesExperiment::compute_elbow_flexion_trajectory(double init_time, double current_time) {
    double t = current_time - init_time;
    double delta = elbow_extended_pos_ - elbow_flexed_pos_;
    double ref_pos = - (6 * delta) * std::pow(t,5) / std::pow(flexion_trajectory_time_,5) + (15 * delta) * std::pow(t,4) / std::pow(flexion_trajectory_time_,4) - (10 * delta) * std::pow(t,3) / std::pow(flexion_trajectory_time_,3) + elbow_extended_pos_;
    ref_pos = mel::saturate(ref_pos,elbow_flexed_pos_,elbow_extended_pos_);

    return ref_pos;  
}

double FesExperiment::compute_elbow_extension_trajectory(double init_time, double current_time) {
    double t = current_time - init_time;
    double delta = elbow_flexed_pos_ - elbow_extended_pos_;
    double ref_pos = -(6 * delta) * std::pow(t, 5) / std::pow(extension_trajectory_time_, 5) + (15 * delta) * std::pow(t, 4) / std::pow(extension_trajectory_time_, 4) - (10 * delta) * std::pow(t, 3) / std::pow(extension_trajectory_time_, 3) + elbow_flexed_pos_;
    ref_pos = mel::saturate(ref_pos, elbow_flexed_pos_, elbow_extended_pos_);

    return ref_pos;
}

void FesExperiment::send_udp_packet(double elbow_pos_deg) {
    UDP_data_[0] = elbow_pos_deg;
    UDP_data_[1] = clock_.time();
    UDP_data_[2] = SUBJECT_NUMBER_;
    UDP_data_[3] = TRIAL_;
    UDP_data_[4] = CONDITION_;
    if (sendto(s_, (char *)&UDP_data_, sizeof(UDP_data_), 0, (struct sockaddr *) &si_other_, slen_) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}