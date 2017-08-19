#include "EmgTraining.h"
#include "Input.h"
#include "Motor.h"



EmgTraining::EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoIIEmgFrc& meii, GuiFlag& gui_flag, int input_mode) :
    StateMachine(7),
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

    // get current position and velocity to apply safety check
    /*q8_emg_->reload_watchdog();
    q8_emg_->read_all();
    meii_.update_kinematics();
    if (meii_.check_all_joint_limits()) {
        stop_ = true;
    }*/

    // transition to next state
    event(ST_PRESENT_TARGET);  

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
    target_check_joint_ = { 1, 0, 0, 0, 0 };
    target_reached_ = false;

    // MelShare unity interface
    target_share_ =  0;

    // enter the control loop
    while (!target_reached_ && !stop_) {

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

        //mel::print(meii_.get_anatomical_joint_velocities());
        vel_share_.write(meii_.get_anatomical_joint_velocities());
        //mel::print(meii_.get_anatomical_joint_positions());
        pos_share_.write(meii_.get_anatomical_joint_positions());

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
        //meii_.set_anatomical_joint_torques(new_torques_);
        
        // write to unity
        target_.write(target_share_);
        pos_data_.write(meii_.get_anatomical_joint_positions());

        // write to daq
        //q8_emg_->write_all();

        // check for target reached
        target_reached_ = check_target_reached(goal_pos_, meii_.get_anatomical_joint_positions(), target_check_joint_,true);

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
        target_reached_ = false;
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

    // MelShare unity interface
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
        //meii_.set_anatomical_joint_torques(new_torques_);

        // write to unity
        target_.write(target_share_);
        pos_data_.write(meii_.get_anatomical_joint_positions());

        // write to daq
        //q8_emg_->write_all();

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
        hold_center_time_reached_ = false;
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

    // MelShare unity interface
    if (current_target_ < target_sequence_.size()) {
        target_share_ = target_sequence_[current_target_];
    }
    else {
        target_share_ = target_sequence_.back();
        end_of_target_sequence_ = true;
    }
    force_share_ = 0;

    // initialize force checking algorithm
    force_mag_maintained_ = 0; // [s]
    force_mag_time_now_ = 0;
    force_mag_time_last_ = 0;

    // initialize emg data buffer
    //boost::circular_buffer<double> emg_data_buffer_(200);
    //emg_data_buffer_ = mel::array_2D<double, 8, 200>(0);

    // enter the control loop
    while (!force_mag_reached_ && !end_of_target_sequence_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();
        q8_ati_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // check joint limits
        /*if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }*/

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
        //meii_.set_anatomical_joint_torques(new_torques_);

        // get measured forces at wrist force sensor
        wrist_forces_ = meii_.wrist_force_sensor_->get_forces();
        wrist_forces_filt_ = multi_lpf_.filter(wrist_forces_);
        force_share_ = 0;
        for (int i = 0; i < 3; ++i) {
            force_share_ += std::pow(wrist_forces_filt_[i], 2);
        }

        // get measured emg voltages
        emg_data_buffer_.push_back(meii_.get_emg_voltages());
        emg_share_.write(emg_data_buffer_.at(0));

        // check force magnitude
        mel::print(force_share_);
        force_mag_reached_ = check_force_mag_reached(force_mag_goal_, force_share_);

        // write to unity
        target_.write(target_share_);
        pos_data_.write(meii_.get_anatomical_joint_positions());
        force_mag_.write(force_share_);

        // write to daq
        //q8_emg_->write_all();

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
    else if (force_mag_reached_) {
        force_mag_reached_ = false;
        current_target_++;
        event(ST_PROCESS_EMG);      
    }
    else if (end_of_target_sequence_) {
        event(ST_FINISH);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "PROCESS EMG DATA" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgTraining::sf_process_emg(const mel::NoEventData* data) {

    mel::print("Process EMG Data");

    // extract features from EMG data
    feature_vec_ = feature_extract(emg_data_buffer_);
    mel::print(feature_vec_);

    stop_ = true;

    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else if (emg_data_processed_) {
        event(ST_HOLD_CENTER);
    }

}



//-----------------------------------------------------------------------------
// "FINISH Experiment" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgTraining::sf_finish(const mel::NoEventData* data) {

    mel::print("Finish Experiment");

    event(ST_STOP);

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

bool EmgTraining::check_force_mag_reached(double force_mag_goal, double force_mag) {
    force_mag_time_now_ = clock_.async_time();
    force_mag_maintained_ = std::abs(force_mag_maintained_ + std::copysign(1.0, force_mag_tol_ - std::abs(force_mag_goal - force_mag)) * (force_mag_time_now_ - force_mag_time_last_));
    force_mag_time_last_ = force_mag_time_now_;
    return force_mag_maintained_ > force_mag_dwell_time_;
}

mel::double_vec EmgTraining::feature_extract(EmgDataBuffer& emg_data_buffer) {

    mel::double_vec feature_vec;
    feature_vec.reserve(num_channels_*num_features_);
    mel::double_vec nrms_vec;
    mel::double_vec nmav_vec;
    mel::double_vec nwl_vec;
    mel::double_vec nzc_vec;
    mel::double_vec nssc_vec;

    // extract unnormalized features
    for (int i = 0; i < emg_data_buffer.num_channels_; ++i) {
        nrms_vec.push_back(rms_feature_extract(emg_data_buffer.data_buffer_[i]));
        nmav_vec.push_back(mav_feature_extract(emg_data_buffer.data_buffer_[i]));
        nwl_vec.push_back(wl_feature_extract(emg_data_buffer.data_buffer_[i]));
        nzc_vec.push_back(zc_feature_extract(emg_data_buffer.data_buffer_[i]));
        nssc_vec.push_back(ssc_feature_extract(emg_data_buffer.data_buffer_[i]));
    }
    
    // normalize features
    double rms_mean = 0;
    double mav_mean = 0;
    double wl_mean = 0;
    double zc_mean = 0;
    double ssc_mean = 0;
    for (int i = 0; i < emg_data_buffer.num_channels_; ++i) {
        rms_mean += nrms_vec[i] / num_channels_;
        mav_mean += nmav_vec[i] / num_channels_;
        wl_mean += nwl_vec[i] / num_channels_;
        zc_mean += nzc_vec[i] / num_channels_;
        ssc_mean += nssc_vec[i] / num_channels_;
    }
    for (int i = 0; i < emg_data_buffer.num_channels_; ++i) {
        nrms_vec[i] = nrms_vec[i] / rms_mean;
        nmav_vec[i] = nmav_vec[i] / mav_mean;
        nwl_vec[i] = nwl_vec[i] / wl_mean;
        if (zc_mean > 0) {
            nzc_vec[i] = nzc_vec[i] / zc_mean;
        }
        if (ssc_mean > 0) {
            nssc_vec[i] = nssc_vec[i] / ssc_mean;
        }
    }

    // copy features into one vector (inserted in reverse order)
    auto it = feature_vec.begin();
    it = feature_vec.insert(it, nssc_vec.begin(), nssc_vec.end());
    it = feature_vec.insert(it, nzc_vec.begin(), nzc_vec.end());
    it = feature_vec.insert(it, nwl_vec.begin(), nwl_vec.end());
    it = feature_vec.insert(it, nmav_vec.begin(), nmav_vec.end());
    it = feature_vec.insert(it, nrms_vec.begin(), nrms_vec.end());
    return feature_vec;
}

double EmgTraining::rms_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_squares = 0;
    for (int i = 0; i < emg_channel_buffer.size(); ++i) {
        sum_squares += std::pow(emg_channel_buffer[i], 2);
    }
    return std::sqrt(sum_squares / emg_channel_buffer.size());
}

double EmgTraining::mav_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs = 0;
    for (int i = 0; i < emg_channel_buffer.size(); ++i) {
        sum_abs += std::abs(emg_channel_buffer[i]);
    }
    return std::sqrt(sum_abs / emg_channel_buffer.size());
}

double EmgTraining::wl_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs_diff = 0;
    for (int i = 0; i < emg_channel_buffer.size()-1; ++i) {
        sum_abs_diff += std::abs(emg_channel_buffer[i + 1] - emg_channel_buffer[i]);
    }
    return sum_abs_diff;
}

double EmgTraining::zc_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs_diff_sign = 0;
    for (int i = 0; i < emg_channel_buffer.size() - 1; ++i) {
        sum_abs_diff_sign += std::abs(std::copysign(1.0,emg_channel_buffer[i + 1]) - std::copysign(1.0,emg_channel_buffer[i]));
    }
    return sum_abs_diff_sign / 2;
}

double EmgTraining::ssc_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs_diff_sign_diff = 0;
    for (int i = 0; i < emg_channel_buffer.size() - 2; ++i) {
        sum_abs_diff_sign_diff += std::abs(std::copysign(1.0,(emg_channel_buffer[i + 2] - emg_channel_buffer[i+1])) - std::copysign(1.0, (emg_channel_buffer[i + 1] - emg_channel_buffer[i])));
    }
    return sum_abs_diff_sign_diff / 2;
}

mel::double_vec EmgTraining::ar4_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    const mel::double_vec x = { 1, 2, 3 };
    // initialize
    mel::double_vec coeffs;
    size_t N = emg_channel_buffer.size();
    size_t m = 4;
    mel::double_vec A_k(m + 1, 0.0);
    A_k[0] = 1.0;
    mel::double_vec f(x);
    mel::double_vec b(x);
    double D_k = 0;
    for (size_t j = 0; j <= N; ++j) {
        D_k += 2.0 * std::pow(f[j], 2);
    }
    D_k -= std::pow(f[0], 2) + std::pow(b[N], 2);

    // Burg recursion
    for (size_t k = 0; k < m; ++k) {
        // compute mu
        double mu = 0.0;
        for (size_t n = 0; n <= N - k - 1; ++n) {
            mu += f[n + k + 1] * b[n];
        }
        mu *= -2.0 / D_k;

        // update A_k
        for (size_t n = 0; n <= (k + 1) / 2; ++n) {
            double t1 = A_k[n] + mu * A_k[k = 1 - n];
            double t2 = A_k[k + 1 - n] + mu * A_k[n];
            A_k[n] = t1;
            A_k[k + 1 - n] = t2;
        }

        // update f and b
        for (size_t n = 0; n <= N - k - 1; ++n) {
            double t1 = f[n + k + 1] + mu * b[n];
            double t2 = b[n] + mu * f[n + k + 1];
            f[n + k + 1] = t1;
            b[n] = t2;
        }

        // update D_k
        D_k = (1.0 - std::pow(mu, 2)) * D_k - std::pow(f[k + 1], 2) - std::pow(b[N - k - 1], 2);
    }
    // assign coefficients
}