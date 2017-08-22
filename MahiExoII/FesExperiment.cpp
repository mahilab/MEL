#include "FesExperiment.h"
#include "Input.h"


FesExperiment::FesExperiment(mel::Clock& clock, mel::Daq* q8_emg, MahiExoII& meii) :
    StateMachine(6),
    clock_(clock),
    q8_emg_(q8_emg),
    meii_(meii)
{
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
    //struct sockaddr_in si_other_;
    //int s_, slen_ = sizeof(si_other_);
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

    // initialize event variables
    st_enter_time_ = clock_.time();
    init_transparent_time_reached_ = false;

    // enter the control loop
    while (!init_transparent_time_reached_ && !stop_) {
    //while (!stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();


        // setting up UDP double array of elbow position and time
        elbow_pos_deg_ = meii_.get_anatomical_joint_position(0) * mel::RAD2DEG;
        current_time_ = clock_.time();
        UDP_data_[0] = elbow_pos_deg_;
        UDP_data_[1] = current_time_;

        // send joint angle and time through UDP
        /*sendto(s_, (char *)&UDP_data_, sizeof(UDP_data_), 0, (struct sockaddr *) &si_other_, slen_);*/
        if (sendto(s_, (char *)&UDP_data_, sizeof(UDP_data_), 0, (struct sockaddr *) &si_other_, slen_) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
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
        init_transparent_time_reached_ = check_wait_time_reached(init_transparent_time_, st_enter_time_, clock_.time());

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
        event(ST_TO_CENTER);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}


//-----------------------------------------------------------------------------
// "GO TO CENTER" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_to_center(const mel::NoEventData* data) {
    mel::print("Go to Center");

    // initialize event variables
    st_enter_time_ = clock_.time();
    target_reached_ = false;

    // get current position and time to initialize trajectory
    q8_emg_->reload_watchdog();
    q8_emg_->read_all();
    meii_.update_kinematics();
    init_pos_ = meii_.get_anatomical_joint_positions();

    // set goal position for trajectory
    goal_pos_ = center_pos_;

    // set joints to be checked for target reached
    target_check_joint_ = { 1, 0, 0, 0, 0 };


    // enter the control loop
    while (!target_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

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
        //mel::print(commanded_torques_[0]);

        // write to daq
        q8_emg_->write_all();

        // check for target reached
        target_reached_ = check_target_reached(goal_pos_, meii_.get_anatomical_joint_positions(), target_check_joint_);

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
    else if (target_reached_) {
        target_reached_ = false; // reset flag
        event(ST_HOLD_CENTER);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "HOLD AT CENTER" STATE FUNCTION
//-----------------------------------------------------------------------------
void FesExperiment::sf_hold_center(const mel::NoEventData* data) {
    mel::print("Hold at Center");

    // initialize event variables
    st_enter_time_ = clock_.time();
    hold_center_time_reached_ = false;

    // initialize trajectory
    init_pos_ = center_pos_;
    goal_pos_ = center_pos_;

    // enter the control loop
    while (!hold_center_time_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

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
        //mel::print(commanded_torques_[0]);

        // write to daq
        q8_emg_->write_all();

        // check for hold time reached
        hold_center_time_reached_ = check_wait_time_reached(hold_center_time_, st_enter_time_, clock_.time());

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
    else if (hold_center_time_reached_) {
        hold_center_time_reached_ = false; // reset flag
        event(ST_FINISH);
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

    mel::print("Finish Experiment");

    event(ST_STOP);

}

//-----------------------------------------------------------------------------
// "STOP" STATE FUNCTION
//-----------------------------------------------------------------------------

void FesExperiment::sf_stop(const mel::NoEventData* data) {
    std::cout << "State Stop " << std::endl;
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
    return (current_time - init_time) > wait_time;
}

bool FesExperiment::check_force_mag_reached(double force_mag_goal, double force_mag) {
    force_mag_time_now_ = clock_.global_time();
    force_mag_maintained_ = std::abs(force_mag_maintained_ + std::copysign(1.0, force_mag_tol_ - std::abs(force_mag_goal - force_mag)) * (force_mag_time_now_ - force_mag_time_last_));
    force_mag_time_last_ = force_mag_time_now_;
    return force_mag_maintained_ > force_mag_dwell_time_;
}