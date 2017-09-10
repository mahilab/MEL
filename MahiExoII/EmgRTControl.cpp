#include "EmgRTControl.h"
#include "Input.h"
#include "Motor.h"

using namespace mel;

EmgRTControl::EmgRTControl(mel::Clock& clock, mel::Daq* q8_emg, MahiExoIIEmg& meii, GuiFlag& gui_flag, int input_mode) :
    StateMachine(9),
    clock_(clock),
    q8_emg_(q8_emg),
    meii_(meii),
    gui_flag_(gui_flag),
    INPUT_MODE_(input_mode)
{
}

void EmgRTControl::wait_for_input() {
    if (INPUT_MODE_ == 0) {
        mel::Input::wait_for_key_press(mel::Input::Key::Space);
    }
    else if (INPUT_MODE_ = 1) {
        gui_flag_.wait_for_flag(1);
        mel::print("");
    }
}

bool EmgRTControl::check_stop() {
    return mel::Input::is_key_pressed(mel::Input::Escape) || (mel::Input::is_key_pressed(mel::Input::LControl) && mel::Input::is_key_pressed(mel::Input::C));
}

//-----------------------------------------------------------------------------
// "INITIALIZATION" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgRTControl::sf_init(const mel::NoEventData* data) {

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
    if (!Q8Usb::check_digital_loopback(0, 7)) {
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
    mel::print("\nPress Enter to run EMG Real-Time Control");
    mel::Input::wait_for_key_press(mel::Input::Key::Return);
    mel::print("\nRunning EMG Real-Time Control ... ");

    // start the watchdog
    q8_emg_->start_watchdog(0.1);

    // start the clock
    clock_.start();

    // transition to next state
    if (stop_) {
        event(ST_STOP);
    }
    else {
        event(ST_TRANSPARENT);
    }
}


//-----------------------------------------------------------------------------
// "TRANSPARENT" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgRTControl::sf_transparent(const mel::NoEventData* data) {
    mel::print("Robot Transparent");

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

        // write kinematics to MelScope
        pos_share_.write(meii_.get_anatomical_joint_positions());
        vel_share_.write(meii_.get_anatomical_joint_velocities());

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // set zero torques
        commanded_torques_ = { 0,0,0,0,0 };
        meii_.set_anatomical_joint_torques(commanded_torques_);

        // write motor commands to MelScope
        torque_share_.write(commanded_torques_);

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
void EmgRTControl::sf_to_center(const mel::NoEventData* data) {
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

    // MelShare unity interface
    target_share_ = 0;

    // enter the control loop
    while (!target_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // write kinematics to MelScope
        pos_share_.write(meii_.get_anatomical_joint_positions());
        vel_share_.write(meii_.get_anatomical_joint_velocities());

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

        // write motor commands to MelScope
        torque_share_.write(commanded_torques_);

        // write to Unity
        target_.write(target_share_);
        //pos_data_.write(meii_.get_anatomical_joint_positions());

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
void EmgRTControl::sf_hold_center(const mel::NoEventData* data) {
    mel::print("Hold at Center");

    // initialize event variables
    st_enter_time_ = clock_.time();
    hold_center_time_reached_ = false;

    // initialize trajectory
    init_pos_ = center_pos_;
    goal_pos_ = center_pos_;

    // MelShare unity interface
    target_share_ = 0;

    // enter the control loop
    while (!hold_center_time_reached_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // write kinematics to MelScope
        pos_share_.write(meii_.get_anatomical_joint_positions());
        vel_share_.write(meii_.get_anatomical_joint_velocities());

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

        // write motor commands to MelScope
        torque_share_.write(commanded_torques_);

        // write to unity
        target_.write(target_share_);
        //pos_data_.write(meii_.get_anatomical_joint_positions());

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
void EmgRTControl::sf_present_target(const mel::NoEventData* data) {
    mel::print("Present Target");

    // initialize event variables
    st_enter_time_ = clock_.time();
    force_mag_reached_ = false;

    // initialize trajectory
    init_pos_ = center_pos_;
    goal_pos_ = center_pos_;

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
    force_mag_time_now_ = clock_.global_time();
    force_mag_time_last_ = clock_.global_time();

    // initialize emg data buffer
    //boost::circular_buffer<double> emg_data_buffer_(200);
    //emg_data_buffer_ = mel::array_2D<double, 8, 200>(0);

    // enter the control loop
    while (!force_mag_reached_ && !end_of_target_sequence_ && !stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // write kinematics to MelScope
        pos_share_.write(meii_.get_anatomical_joint_positions());
        vel_share_.write(meii_.get_anatomical_joint_velocities());

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        // compute pd torques
        init_time_ = 0;
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

        // write motor commands to MelScope
        torque_share_.write(commanded_torques_);


        // artificial force input
        force_share_ = 1000.0;

        // get measured emg voltages
        // TO DO: add in band pass filtering
        emg_data_buffer_.push_back(meii_.get_emg_voltages());
        emg_share_.write(emg_data_buffer_.at(0));

        // check force magnitude
        force_mag_reached_ = check_force_mag_reached(force_mag_goal_, force_share_);

        // write to unity
        target_.write(target_share_);
        //pos_data_.write(meii_.get_anatomical_joint_positions());
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
    else if (force_mag_reached_) {
        force_mag_reached_ = false; // reset flag
        current_target_++;
        event(ST_PROCESS_EMG);
    }
    else if (end_of_target_sequence_) {
        event(ST_TRAIN_CLASSIFIER);
    }
    else {
        mel::print("ERROR: State transition undefined. Going to ST_STOP.");
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// "PROCESS EMG DATA" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgRTControl::sf_process_emg(const mel::NoEventData* data) {
    mel::print("Process EMG Data");

    // extract features from EMG data
    feature_vec_ = feature_extract(emg_data_buffer_);

    // store features in training data set
    std::copy_n(feature_vec_.begin(), feature_array_.size(), feature_array_.begin());
    emg_training_data_.push_back(feature_array_);

    emg_data_processed_ = true;
    // transition to next state
    if (stop_) {
        // stop if user provided input
        event(ST_STOP);
    }
    else if (emg_data_processed_) {
        emg_data_processed_ = false; // reset flag
        event(ST_HOLD_CENTER);
    }
}


//-----------------------------------------------------------------------------
// "TRAIN CLASSIFIER" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgRTControl::sf_train_classifier(const mel::NoEventData* data) {
    
    mel::print("Training Complete");

    meii_.disable();

    //open LDA script in Python
    //system("start EMG_FS_LDA.py &");

    // create vector to send training data to python
    mel::double_vec emg_training_data_vec_(N_train_ * num_emg_channels_ * num_features_);
    for (int i = 0; i < N_train_; ++i) {
        std::copy_n(emg_training_data_[i].begin(), num_emg_channels_ * num_features_, emg_training_data_vec_.begin());
    }

    // write training data to python
    std::array<int, 2> training_data_size = {N_train_, num_emg_channels_ * num_features_};
    trng_size_.write(training_data_size);
    trng_share_.write(emg_training_data_vec_);
    std::vector<int> training_labels(N_train_);
    std::copy_n(target_sequence_.begin(), N_train_, training_labels.begin());
    label_share_.write(training_labels);

    std::array<int, 2> training_data_size2;
    std::vector<int> sel_feats(training_data_size2[1]);
    mel::double_vec lda_coeff_c(num_class_, training_data_size2[1]); //replaced 2nd index (num_emg_channels_ * num_features_)

    // wait for python to receive
    // restart the clock
    clock_.start();
    while (!stop_) {

        // read and reload DAQs
        q8_emg_->reload_watchdog();
        q8_emg_->read_all();

        // update robot kinematics
        meii_.update_kinematics();

        // write kinematics to MelScope
        pos_share_.write(meii_.get_anatomical_joint_positions());
        vel_share_.write(meii_.get_anatomical_joint_velocities());

        // check joint limits
        if (meii_.check_all_joint_limits()) {
            stop_ = true;
            break;
        }

        //read for number of selected features from Python
        trng_size2_.read(training_data_size2);

        //read for selected feature indicies from Python
        feat_id_.read(sel_feats);

        //read for LDA Coefficients from Python
        lda_coeff_.read(lda_coeff_c);
        //mel::print(lda_coeff_c);

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    event(ST_FINISH);
}

//-----------------------------------------------------------------------------
// "FINISH Experiment" STATE FUNCTION
//-----------------------------------------------------------------------------
void EmgRTControl::sf_finish(const mel::NoEventData* data) {

    mel::print("Finish Experiment");

    event(ST_STOP);

}

//-----------------------------------------------------------------------------
// "STOP" STATE FUNCTION
//-----------------------------------------------------------------------------

void EmgRTControl::sf_stop(const mel::NoEventData* data) {
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

bool EmgRTControl::check_target_reached(mel::double_vec goal_pos, mel::double_vec current_pos, mel::char_vec target_check_joint, bool print_output) {

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

bool EmgRTControl::check_wait_time_reached(double wait_time, double init_time, double current_time) {
    return (current_time - init_time) > wait_time;
}

bool EmgRTControl::check_force_mag_reached(double force_mag_goal, double force_mag) {
    force_mag_time_now_ = clock_.global_time();
    force_mag_maintained_ = std::abs(force_mag_maintained_ + std::copysign(1.0, force_mag_tol_ - std::abs(force_mag_goal - force_mag)) * (force_mag_time_now_ - force_mag_time_last_));
    force_mag_time_last_ = force_mag_time_now_;
    return force_mag_maintained_ > force_mag_dwell_time_;
}

mel::double_vec EmgRTControl::feature_extract(EmgDataBuffer& emg_data_buffer) {

    mel::double_vec feature_vec;
    feature_vec.reserve(num_emg_channels_*num_features_);
    mel::double_vec nrms_vec(num_emg_channels_, 0.0);
    mel::double_vec nmav_vec(num_emg_channels_, 0.0);
    mel::double_vec nwl_vec(num_emg_channels_, 0.0);
    mel::double_vec nzc_vec(num_emg_channels_, 0.0);
    mel::double_vec nssc_vec(num_emg_channels_, 0.0);
    mel::double_vec ar_vec(4,0.0);
    mel::double_vec ar1_vec(num_emg_channels_, 0.0);
    mel::double_vec ar2_vec(num_emg_channels_, 0.0);
    mel::double_vec ar3_vec(num_emg_channels_, 0.0);
    mel::double_vec ar4_vec(num_emg_channels_, 0.0);

    

    // extract unnormalized features
    for (int i = 0; i < num_emg_channels_; ++i) {
        nrms_vec[i] = rms_feature_extract(emg_data_buffer.data_buffer_[i]);        
        nmav_vec[i] = mav_feature_extract(emg_data_buffer.data_buffer_[i]);
        nwl_vec[i] = wl_feature_extract(emg_data_buffer.data_buffer_[i]);
        nzc_vec[i] = zc_feature_extract(emg_data_buffer.data_buffer_[i]);
        nssc_vec[i] = ssc_feature_extract(emg_data_buffer.data_buffer_[i]);
        ar4_feature_extract(ar_vec, emg_data_buffer.get_channel(i));
        ar1_vec[i] = ar_vec[0];
        ar2_vec[i] = ar_vec[1];
        ar3_vec[i] = ar_vec[2];
        ar4_vec[i] = ar_vec[3];
    }
    

    // normalize features
    double rms_mean = 0.0;
    double mav_mean = 0.0;
    double wl_mean = 0.0;
    double zc_mean = 0.0;
    double ssc_mean = 0.0;
    for (int i = 0; i < num_emg_channels_; ++i) {
        rms_mean += nrms_vec[i] / num_emg_channels_;
        mav_mean += nmav_vec[i] / num_emg_channels_;
        wl_mean += nwl_vec[i] / num_emg_channels_;
        zc_mean += nzc_vec[i] / num_emg_channels_;
        ssc_mean += nssc_vec[i] / num_emg_channels_;
    }
    for (int i = 0; i < num_emg_channels_; ++i) {
        if (rms_mean > 0) {
            nrms_vec[i] = nrms_vec[i] / rms_mean;
        }
        if (mav_mean > 0) {
            nmav_vec[i] = nmav_vec[i] / mav_mean;
        }
        if (wl_mean > 0) {
            nwl_vec[i] = nwl_vec[i] / wl_mean;
        }
        if (zc_mean > 0) {
            nzc_vec[i] = nzc_vec[i] / zc_mean;
        }
        if (ssc_mean > 0) {
            nssc_vec[i] = nssc_vec[i] / ssc_mean;
        }
    }

    // copy features into one vector (inserted in reverse order)
    auto it = feature_vec.begin();
    it = feature_vec.insert(it, ar4_vec.begin(), ar4_vec.end());
    it = feature_vec.insert(it, ar3_vec.begin(), ar3_vec.end());
    it = feature_vec.insert(it, ar2_vec.begin(), ar2_vec.end());
    it = feature_vec.insert(it, ar1_vec.begin(), ar1_vec.end());
    it = feature_vec.insert(it, nssc_vec.begin(), nssc_vec.end());
    it = feature_vec.insert(it, nzc_vec.begin(), nzc_vec.end());
    it = feature_vec.insert(it, nwl_vec.begin(), nwl_vec.end());
    it = feature_vec.insert(it, nmav_vec.begin(), nmav_vec.end());
    it = feature_vec.insert(it, nrms_vec.begin(), nrms_vec.end());
    return feature_vec;
}

double EmgRTControl::rms_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_squares = 0;
    for (int i = 0; i < emg_channel_buffer.size(); ++i) {
        sum_squares += std::pow(emg_channel_buffer[i], 2);
    }
    return std::sqrt(sum_squares / emg_channel_buffer.size());
}

double EmgRTControl::mav_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs = 0;
    for (int i = 0; i < emg_channel_buffer.size(); ++i) {
        sum_abs += std::abs(emg_channel_buffer[i]);
    }
    return sum_abs / emg_channel_buffer.size();
}

double EmgRTControl::wl_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs_diff = 0.0;
    for (int i = 0; i < emg_channel_buffer.size() - 1; ++i) {
        
        sum_abs_diff += std::abs(emg_channel_buffer[i + 1] - emg_channel_buffer[i]);
    }
    return sum_abs_diff;
}

double EmgRTControl::zc_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs_diff_sign = 0;
    for (int i = 0; i < emg_channel_buffer.size() - 1; ++i) {
        sum_abs_diff_sign += std::abs(std::copysign(1.0, emg_channel_buffer[i + 1]) - std::copysign(1.0, emg_channel_buffer[i]));
    }
    return sum_abs_diff_sign / 2;
}

double EmgRTControl::ssc_feature_extract(boost::circular_buffer<double> emg_channel_buffer) {
    double sum_abs_diff_sign_diff = 0;
    for (int i = 0; i < emg_channel_buffer.size() - 2; ++i) {
        sum_abs_diff_sign_diff += std::abs(std::copysign(1.0, (emg_channel_buffer[i + 2] - emg_channel_buffer[i + 1])) - std::copysign(1.0, (emg_channel_buffer[i + 1] - emg_channel_buffer[i])));
    }
    return sum_abs_diff_sign_diff / 2;
}

void EmgRTControl::ar4_feature_extract(mel::double_vec& coeffs, const mel::double_vec& emg_channel_buffer) {

    // initialize
    size_t N = emg_channel_buffer.size();
    size_t m = coeffs.size();
    mel::double_vec A_k(m + 1, 0.0);
    A_k[0] = 1.0;
    mel::double_vec f(emg_channel_buffer);
    mel::double_vec b(emg_channel_buffer);
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
            double t1 = A_k[n] + mu * A_k[k + 1 - n];
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
    coeffs.assign(++A_k.begin(), A_k.end());
}