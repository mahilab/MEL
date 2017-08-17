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

    // get current position
    q8_emg_->read_all();
    meii_.update_kinematics();
    init_pos_ = meii_.get_anatomical_joint_positions();
    goal_pos_ = init_pos_;

    // transition to next state
    event(ST_TO_CENTER);

    // entry into next state
    goal_pos_[0] = -35 * mel::DEG2RAD; // elbow neutral [rad]
    goal_pos_[1] = 0 * mel::DEG2RAD; // forearm neutral [rad]
    goal_pos_[2] = 0 * mel::DEG2RAD; // wrist f/e neutral [rad]
    goal_pos_[3] = 0 * mel::DEG2RAD; // wrist r/u neutral [rad]
    goal_pos_[4] = 0.09; // arm translation neutral [rad]

}

void EmgTraining::sf_to_center(const mel::NoEventData* data) {
    
    // reset and start the hardware clock
    clock_.start();

    // enter the control loop
    while (!stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();
        q8_ati_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // compute pd torques
        double arm_translation_force = 1;
        init_time_ = 0;
        for (auto i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], init_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            //if (i == 4) {
            //    arm_translation_force = new_torques_[i];
            //}
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }
        //mel::print(arm_translation_force);

        // write position data (UNITY DEBUG)
        pos_data_.write(meii_.get_anatomical_joint_positions());
        
        //mel::double_vec printable = { new_torques_[0],static_cast<mel::Motor*>(meii_.joints_[0]->actuator_)->get_current_limited() };
        //mel::print(printable);
        
        // set new torques
        meii_.set_anatomical_joint_torques(new_torques_);
        
        // write to unity
        std::array<int, 1> target_share = { 0 };
        target_.write(target_share);


        // write to daq
        q8_emg_->write_all();

        // transition to next state
        event(ST_TO_CENTER);

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    if (stop_) {
        event(ST_STOP);
    }
}

void EmgTraining::sf_hold_center(const mel::NoEventData* data) {

    // reset and start the hardware clock
    clock_.start();

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

        //force_share_ = meii_->wrist_force_sensor_->get_forces();
        //mel::print(force_share_);

        //mel::print(meii_->get_emg_voltages());

        //force_filt_ = multi_lpf_.filter(force_share_);

        //ati_data_.write(force_filt_);

        // compute pd torques
        init_time_ = 0;
        for (auto i = 0; i < 5; ++i) {
            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], init_time_, clock_.time(), speed_[i]);
            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_.get_anatomical_joint_position(i), 0, meii_.get_anatomical_joint_velocity(i));
            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }
        }

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    if (stop_) {
        event(ST_STOP);
    }
}

void EmgTraining::sf_present_target(const mel::NoEventData* data) {

}

void EmgTraining::sf_stop(const mel::NoEventData* data) {
    std::cout << "State Stop " << std::endl;
    meii_.disable();
    q8_emg_->disable();
    q8_ati_->disable();
}
