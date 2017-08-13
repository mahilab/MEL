#include "EmgTraining.h"
#include "Input.h"



EmgTraining::EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoIIEmg* meii) :
    StateMachine(4),
    clock_(clock),
    q8_emg_(q8_emg),
    q8_ati_(q8_ati),
    meii_(meii)
{ 
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
    /*mel::print("\nPress Enter to enable MEII.");
    mel::Input::wait_for_key_press(mel::Input::Key::Return);
    meii_->enable();
    if (!meii_->is_enabled()) {
        event(ST_STOP);
        return;
    }*/

    
    std::cout << "Press Enter to start the controller" << std::endl;
    getchar();
    q8_emg_->start_watchdog(0.1);
    std::cout << "Starting the controller ... " << std::endl;

    // get current position
    q8_emg_->read_all();
    meii_->update_kinematics();
    init_pos_ = meii_->get_anatomical_joint_positions();
    goal_pos_ = init_pos_;

    /*
    //write position data (UNITY DEBUG)
    data_p_ = meii_->get_anatomical_joint_positions();
    pos_data_.write(data_p_);
    mel::print(data_p_);
    */

    // transition to next state
    event(ST_HOLD_NEUTRAL);

    // entry into next state
    goal_pos_[0] = -0.6; // elbow neutral
    goal_pos_[1] = 0; // forearm neutral
    goal_pos_[2] = 0; // wrist f/e neutral
    goal_pos_[3] = 0; // wrist r/u neutral
    goal_pos_[4] = 0.09; // arm translation neutral

}

void EmgTraining::sf_to_neutral(const mel::NoEventData* data) {
    
    // reset and start the hardware clock
    clock_.start();


    // enter the control loop
    /*while (!ctrl_c_) {

        q8_emg_->reload_watchdog();

        q8_emg_->read_all();


        meii_->update_kinematics();

        // set torques
        init_time_ = 0;
        for (auto i = 0; i < 5; ++i) {

            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], init_time_, clock_.time(), speed_[i]);

            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], meii_->get_anatomical_joint_position(i), 0, meii_->get_anatomical_joint_velocity(i));

            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }

        }
        
        data_p_ = meii_->get_anatomical_joint_positions();
        pos_data_.write(data_p_);
        mel::print(data_p_);

        //mel::print(meii_->get_anatomical_joint_position(0));
        //mel::print(new_torques_[2]);
        meii_->set_anatomical_joint_torques(new_torques_);
        q8_emg_->write_all();
    }*/

}

void EmgTraining::sf_hold_neutral(const mel::NoEventData* data) {


    // reset and start the hardware clock
    clock_.start();


    // enter the control loop
    //while (!ctrl_c_) {


        q8_emg_->reload_watchdog();

        q8_emg_->read_all();
        q8_ati_->read_all();

        /*std::vector<double> v = q8_ati_->get_analog_voltages();
        double* ptr = &v[0];
        Eigen::Map<Eigen::VectorXd> raw_force(ptr, 6);

        raw_force_ = raw_force;

        calib_force_ = calib_mat_*raw_force_;
        force_share_.resize(calib_force_.size());
        Eigen::VectorXd::Map(&force_share_[0], calib_force_.size()) = calib_force_;*/

        //force_share_ = meii_->wrist_force_sensor_->get_forces();
        //mel::print(force_share_);

        mel::print(meii_->get_emg_voltages());

        //force_filt_ = multi_lpf_.filter(force_share_);

        //ati_data_.write(force_filt_);
    //}

}

void EmgTraining::sf_stop(const mel::NoEventData* data) {
    std::cout << "State Stop " << clock_.time() << std::endl;
    
}

/*
void EmgTraining::ctrl_c_task() {
    std::cout << "Program aborted" << std::endl;
    q8_emg_->disable();
    q8_ati_->disable();
}
*/