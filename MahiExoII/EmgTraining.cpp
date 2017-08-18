#include "EmgTraining.h"
#include "Input.h"
#include "Motor.h"



EmgTraining::EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoIIEmgFrc& meii, GuiFlag& gui_flag, int input_mode) :
    StateMachine(5),
    clock_(clock),
    q8_emg_(q8_emg),
    q8_ati_(q8_ati),
    meii_(meii),
    gui_flag_(gui_flag),
    INPUT_MODE_(input_mode)
{ 
}

void EmgTraining::wait_for_input() {
    if (INPUT_MODE_ == 0) {
        mel::Input::wait_for_key_press(mel::Input::Key::Space);
    }
    else if (INPUT_MODE_ = 1) {
        gui_flag_.wait_for_flag(1);
        mel::print("");
    }
}

bool EmgTraining::check_stop() {
    return mel::Input::is_key_pressed(mel::Input::Escape) || (mel::Input::is_key_pressed(mel::Input::LControl) && mel::Input::is_key_pressed(mel::Input::C));
}

//-----------------------------------------------------------------------------
// "INITIALIZATION" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgTraining::sf_init(const mel::NoEventData* data) {

    // enable MEII EMG DAQ
    mel::print("\nPress Enter to enable MEII EMG Daq <" + q8_emg_->name_ + ">.");
    mel::Input::wait_for_key_press(mel::Input::Key::Return);
    q8_emg_->enable();
    if (!q8_emg_->is_enabled()) {
        event(ST_STOP);
        return;
    }

    // enable MEII ATI DAQ
    mel::print("\nPress Enter to enable MEII ATI Daq <" + q8_ati_->name_ + ">.");
    mel::Input::wait_for_key_press(mel::Input::Key::Return);
    q8_ati_->enable();
    if (!q8_ati_->is_enabled()) {
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

    // start the hardware clock
    clock_.start();

    // transition to next state
    event(ST_TO_CENTER);  

}

//-----------------------------------------------------------------------------
// "GO TO CENTER" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgTraining::sf_to_center(const mel::NoEventData* data) {

    mel::print("Go to Center");
    
    // get current position and time to initialize trajectory
    q8_emg_->reload_watchdog();
    q8_emg_->read_all();
    meii_.update_kinematics();
    init_pos_ = meii_.get_anatomical_joint_positions();
    init_time_ = clock_.time();
    //double arm_translation_force = 1; // for debugging

    // set goal position for trajectory
    goal_pos_[0] = -35 * mel::DEG2RAD; // elbow neutral [rad]
    goal_pos_[1] = 0 * mel::DEG2RAD; // forearm neutral [rad]
    goal_pos_[2] = 0 * mel::DEG2RAD; // wrist f/e neutral [rad]
    goal_pos_[3] = 0 * mel::DEG2RAD; // wrist r/u neutral [rad]
    goal_pos_[4] = 0.09; // arm translation neutral [rad]

    // set joints to be checked for target reached
    target_check_joint_ = { 1, 1, 1, 1, 0 };
    target_reached_ = false;

    // unity controls
    target_share_ =  0;

    // enter the control loop
    while (!target_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();
        q8_ati_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute pd torques
        for (auto i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], init_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            //if (i == 4) {
            //    arm_translation_force = new_torques_[i]; // for debugging
            //}
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }
        //mel::print(arm_translation_force);
        
        //mel::double_vec printable = { new_torques_[0],static_cast<mel::Motor*>(meii_.joints_[0]->actuator_)->get_current_limited() };
        //mel::print(printable);
        
        // set new torques
        meii_.set_anatomical_joint_torques(new_torques_);
        
        // write to unity
        target_.write(target_share_);
        pos_data_.write(meii_.get_anatomical_joint_positions());

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
void EmgTraining::sf_hold_center(const mel::NoEventData* data) {

    mel::print("Hold at Center");

    // restart the clock
    clock_.start();

    // initialize trajectory
    goal_pos_[0] = -35 * mel::DEG2RAD; // elbow neutral [rad]
    goal_pos_[1] = 0 * mel::DEG2RAD; // forearm neutral [rad]
    goal_pos_[2] = 0 * mel::DEG2RAD; // wrist f/e neutral [rad]
    goal_pos_[3] = 0 * mel::DEG2RAD; // wrist r/u neutral [rad]
    goal_pos_[4] = 0.09; // arm translation neutral [rad]
    //q8_emg_->reload_watchdog();
    //q8_emg_->read_all();
    //meii_.update_kinematics();
    //init_pos_ = meii_.get_anatomical_joint_positions();
    init_pos_ = goal_pos_;
    init_time_ = clock_.time();

    hold_center_time_reached_ = false;

    // unity controls
    target_share_ = 0;

    // enter the control loop
    while (!hold_center_time_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();
        q8_ati_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // compute pd torques
        init_time_ = 0;
        for (auto i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], init_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // set new torques
        meii_.set_anatomical_joint_torques(new_torques_);

        // write to unity
        target_.write(target_share_);
        pos_data_.write(meii_.get_anatomical_joint_positions());

        // write to daq
        q8_emg_->write_all();

        // check for hold time reached
        hold_center_time_reached_ = check_wait_time_reached(hold_center_time_,init_time_,clock_.time());

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
        event(ST_PRESENT_TARGET);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "PRESENT TARGET" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgTraining::sf_present_target(const mel::NoEventData* data) {

    mel::print("Present Target");

    // initialize trajectory
    goal_pos_[0] = -35 * mel::DEG2RAD; // elbow neutral [rad]
    goal_pos_[1] = 0 * mel::DEG2RAD; // forearm neutral [rad]
    goal_pos_[2] = 0 * mel::DEG2RAD; // wrist f/e neutral [rad]
    goal_pos_[3] = 0 * mel::DEG2RAD; // wrist r/u neutral [rad]
    goal_pos_[4] = 0.09; // arm translation neutral [rad]
             
    // restart the clock
    clock_.start();

    init_pos_ = goal_pos_;
    init_time_ = clock_.time();

    // unity controls
    target_share_ = 1;

    // enter the control loop
    while (!stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();
        q8_ati_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // compute pd torques
        init_time_ = 0;
        for (auto i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], init_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // set new torques
        meii_.set_anatomical_joint_torques(new_torques_);

        // get measured forces at wrist force sensor
        wrist_forces_ = meii_.wrist_force_sensor_->get_forces();
        wrist_forces_filt_ = multi_lpf_.filter(wrist_forces_);
        force_share_ = 0;
        for (int i = 0; i < 3; ++i) {
            force_share_ += std::pow(wrist_forces_filt_[i], 2);
        }
        mel::print(force_share_);

        // write to unity
        target_.write(target_share_);
        pos_data_.write(meii_.get_anatomical_joint_positions());
        force_mag_.write(force_share_);

        // write to daq
        q8_emg_->write_all();

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
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}


//-----------------------------------------------------------------------------
// "STOP" STATE FUNCTION
//-----------------------------------------------------------------------------

void EmgTraining::sf_stop(const mel::NoEventData* data) {
    std::cout << "State Stop " << std::endl;
    meii_.disable();
    q8_emg_->disable();
    q8_ati_->disable();
}

//-----------------------------------------------------------------------------
// UTILITY FUNCTIONS
//-----------------------------------------------------------------------------

bool EmgTraining::check_target_reached(mel::double_vec goal_pos, mel::double_vec current_pos, mel::char_vec target_check_joint, bool print_output) {

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

bool EmgTraining::check_wait_time_reached(double wait_time, double init_time, double current_time) {
    return (current_time - init_time) > wait_time;
}