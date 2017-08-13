#include "HapticGuidance.h"
#include "Input.h"

HapticGuidance::HapticGuidance(mel::Clock& clock, mel::Daq* ow_daq, OpenWrist& open_wrist, Cuff& cuff, GuiFlag& gui_flag, int input_mode,
    int subject_number, int condition, std::string start_trial):
    StateMachine(8), 
    clock_(clock),
    ow_daq_(ow_daq),
    open_wrist_(open_wrist), 
    cuff_(cuff),
    gui_flag_(gui_flag),
    INPUT_MODE_(input_mode),
    SUBJECT_NUMBER_(subject_number),
    CONDITION_(condition)
{
    if (subject_number < 10)
        DIRECTORY_ = "S0" + std::to_string(subject_number) + "_C" + std::to_string(condition);
    else
        DIRECTORY_ = "S" + std::to_string(subject_number) + "_C" + std::to_string(condition);

    build_experiment();

    for (int i = 0; i < TRIALS_TAG_NAMES_.size(); ++i) {
        if (start_trial == TRIALS_TAG_NAMES_[i])
            current_trial_index_ = i-1;
    }

    perlin_module_.SetOctaveCount(1.0);
    perlin_module_.SetFrequency(1.0);
    perlin_module_.SetPersistence(0.1);

    log_.add_col("Time [s]");
}

void HapticGuidance::wait_for_input() {
    if (INPUT_MODE_ == 0) {
        mel::Input::wait_for_key_press(mel::Input::Key::Space);
    }
    else if (INPUT_MODE_ = 1) {
        gui_flag_.wait_for_flag(1);
        mel::print("");
    }
}

void HapticGuidance::allow_continue_input() {
    if (INPUT_MODE_ == 1)
        gui_flag_.reset_flag(0);
}

bool HapticGuidance::check_stop() {
    return mel::Input::is_key_pressed(mel::Input::Escape) || (mel::Input::is_key_pressed(mel::Input::LControl) && mel::Input::is_key_pressed(mel::Input::C));
}


void HapticGuidance::build_experiment() {
    for (auto it = BLOCK_ORDER_.begin(); it != BLOCK_ORDER_.end(); ++it) {
        NUM_BLOCKS_[*it] += 1;
        for (int i = 0; i < NUM_TRIALS_[*it]; i++) {
            TRIALS_BLOCK_TYPES_.push_back(*it);
            TRIALS_BLOCK_NAMES_.push_back(BLOCK_NAMES_[*it]);
            TRIALS_TAG_NAMES_.push_back(BLOCK_TAGS_[*it] + std::to_string(NUM_BLOCKS_[*it]) + "-" + std::to_string(i + 1));
            NUM_TRIALS_TOTAL_ += 1;
        }
    }
}

//-----------------------------------------------------------------------------
// START STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_start(const mel::NoEventData*) {
    
    // enable OpenWrist DAQ
    if (CONDITION_ > 0) {
        mel::print("\nPress Enter to enable OpenWrist Daq <" + ow_daq_->name_ + ">.");
        mel::Input::wait_for_key_press(mel::Input::Key::Return);
        ow_daq_->enable();
        if (!ow_daq_->is_enabled()) {
            event(ST_STOP);
            return;
        }
    }
    
    // enable and pretension CUFF
    if (CONDITION_ == 2 || CONDITION_ == 3) {
        std::cout << "\nPress Enter to enable and pretension CUFF" << std::endl;
        mel::Input::wait_for_key_press(mel::Input::Key::Return);
        cuff_.enable();
        if (!cuff_.is_enabled()) {
            event(ST_STOP);
            return;
        }
        cuff_.pretensioning(CUFF_NORMAL_FORCE_, offset, scaling_factor);
    } 

    // enable MahiExo-II DAQ
    if (CONDITION_ == 4) {
        mel::print("\nPress Enter to enable MahiExo-II Daq <" + ow_daq_->name_ + ">.");
        mel::Input::wait_for_key_press(mel::Input::Key::Return);
    }
    
    event(ST_TRANSITION);   
}

//-----------------------------------------------------------------------------
// FAMILIARIZATION STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_familiarization(const mel::NoEventData*) {

    // show/hide Unity elements
    update_unity(true, true, true, true, true, true, true);

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[FAMILIARIZATION] && !stop_) {

        // update trajectory
        update_trajectory(clock_.time());

        // update expert position
        update_expert(clock_.time());

        // read and reload DAQ
        if (CONDITION_ > 0) {

            // read and reload DAQ
            ow_daq_->reload_watchdog();
            ow_daq_->read_all();

            // step the pendulum simuation
            pendulum_.step_simulation(clock_.time(), open_wrist_.joints_[0]->get_position(), open_wrist_.joints_[0]->get_velocity());

            // compute anglular error
            double error = compute_trajectory_error(open_wrist_.joints_[0]->get_position());

            // set device forces based on conditions
            double ps_torque = open_wrist_.compute_gravity_compensation(0) + 0.75 * open_wrist_.compute_friction_compensation(0) - pendulum_.Tau[0];
            if (CONDITION_ == 1) {

            }
            else if (CONDITION_ == 2) {
                double noise = perlin_module_.GetValue(clock_.time(), 0.0, 0.0);
                cuff_.set_motor_positions((short int)(noise * CUFF_NOISE_GAIN_) + offset[0], (short int)(noise * CUFF_NOISE_GAIN_) + offset[1], true);
            }
            else if (CONDITION_ == 3) {
                cuff_.set_motor_positions((short int)(-error * CUFF_GUIDANCE_GAIN_ + offset[0]), (short int)(-error * CUFF_GUIDANCE_GAIN_ + offset[1]), true);
            } 
            else if (CONDITION_ == 4) {

            }
            open_wrist_.joints_[0]->set_torque(ps_torque);
            open_wrist_.joints_[1]->set_torque(mel::pd_controller(40, 1.0, 0, open_wrist_.joints_[1]->get_position(), 0, open_wrist_.joints_[1]->get_velocity()));

            // update OpenWrist state
            open_wrist_.update_state_map();

            // write the DAQ
            ow_daq_->write_all();
        }

        // log data
        log_data_ = { clock_.time() };
        log_.add_row(log_data_);

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();        
    }

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// EVALUATION STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_evaluation(const mel::NoEventData*) {

    // show/hide Unity elements
    update_unity(true, true, true, false, false, false, true);
   
    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[EVALUATION] && !stop_) {

        // update trajectory
        update_trajectory(clock_.time());

        // update expert position
        update_expert(clock_.time());

        if (CONDITION_ > 0) {
            // read and reload DAQ
            ow_daq_->reload_watchdog();
            ow_daq_->read_all();
        }

        // log data
        log_data_ = { clock_.time() };
        log_.add_row(log_data_);

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// TRAINING STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_training(const mel::NoEventData*) {

    // show/hide Unity elements
    update_unity(true, true, true, false, false, false, true);

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[TRAINING] && !stop_) {

        // update trajectory
        update_trajectory(clock_.time());

        // update expert position
        update_expert(clock_.time());

        if (CONDITION_ > 0) {
            // read and reload DAQ
            ow_daq_->reload_watchdog();
            ow_daq_->read_all();
        }

        // log data
        log_data_ = { clock_.time() };
        log_.add_row(log_data_);

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// BREAK STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_break(const mel::NoEventData*) {

    // show/hide Unity elements
    update_unity(true, false, false, false, false, false, true);

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[BREAK] && !stop_) {

        if (CONDITION_ > 0) {
            // read and reload DAQ
            ow_daq_->reload_watchdog();
            ow_daq_->read_all();
        }

        // log data
        log_data_ = { clock_.time() };
        log_.add_row(log_data_);

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// GENERALIZATION STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_generalization(const mel::NoEventData*) {

    // show/hide Unity elements
    update_unity(true, true, true, false, false, false, true);
    
    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[EVALUATION] && !stop_) {

        // update trajectory
        update_trajectory(clock_.time());

        // update expert position
        update_expert(clock_.time());

        if (CONDITION_ > 0) {
            // read and reload DAQ
            ow_daq_->reload_watchdog();
            ow_daq_->read_all();
        }

        // log data
        log_data_ = { clock_.time() };
        log_.add_row(log_data_);

        // check for stop input
        stop_ = check_stop();

        // wait for the next clock cycle
        clock_.wait();
    }

    // transition to the next state
    event(ST_TRANSITION);

}

//-----------------------------------------------------------------------------
// TRANSITION STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_transition(const mel::NoEventData*) {

    // suspend hardware
    if (CONDITION_ > 0) {
        open_wrist_.disable();
        ow_daq_->stop_watchdog();
    }

    // show/hide Unity elements
    update_unity(true, false, false, false, false, false, true);

    // save the data log from the last trial
    if (trials_started_) {
        log_.save_and_clear_data(TRIALS_TAG_NAMES_[current_trial_index_], DIRECTORY_ + "\\_" + TRIALS_BLOCK_NAMES_[current_trial_index_], true);
    }

    // start a new tiral if there is one or stop hasn't been requested
    if (current_trial_index_ < NUM_TRIALS_TOTAL_ - 1 && !stop_) {

        // increment the trial;
        current_trial_index_ += 1;
        mel::print("\nNEXT TRIAL: <" + TRIALS_TAG_NAMES_[current_trial_index_] + ">. Press Space to begin.");
        while (!mel::Input::is_key_pressed(mel::Input::Space)) {
            stop_ = check_stop();
            if (stop_) {
                event(ST_STOP);
                return;
            }
        }

        trials_started_ = true;

        // resume hardware
        if (CONDITION_ > 0) {
            open_wrist_.enable();
            ow_daq_->start_watchdog(0.1);
        }

        // print message
        mel::print("STARTING TRIAL: <" + TRIALS_TAG_NAMES_[current_trial_index_] + ">. Press Escape or Ctrl+C to terminate the experiment.");

        // restart the clock
        clock_.start();

        // transition to the next state
        if (TRIALS_BLOCK_TYPES_[current_trial_index_] == FAMILIARIZATION)
            event(ST_FAMILIARIZATION);
        else if (TRIALS_BLOCK_TYPES_[current_trial_index_] == EVALUATION)
            event(ST_EVALUATION);
        else if (TRIALS_BLOCK_TYPES_[current_trial_index_] == TRAINING)
            event(ST_TRAINING);
        else if (TRIALS_BLOCK_TYPES_[current_trial_index_] == BREAK)
            event(ST_BREAK);
        else if (TRIALS_BLOCK_TYPES_[current_trial_index_] == GENERALIZATION)
            event(ST_GENERALIZATION);
        return;
    }    
    else {
        event(ST_STOP);
        return;
    }
}

//-----------------------------------------------------------------------------
// STOP STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_stop(const mel::NoEventData*) {
    if (current_trial_index_ < 0)
        mel::print("\nExperiment terminated during startup. Disabling hardware.");
    else if (current_trial_index_ < NUM_TRIALS_TOTAL_ - 1)
        mel::print("\nExperiment terminated during trial " + mel::namify(TRIALS_TAG_NAMES_[current_trial_index_ ]) + ". Disabling hardware.");
    else
        mel::print("\nExperiment completed. Disabling hardware.");

    if (CONDITION_ > 0)
        ow_daq_->disable();

    if (CONDITION_ == 2 || CONDITION_ == 3)
        cuff_.disable();
}

//-----------------------------------------------------------------------------
// UTILITY FUNCTIONS
//-----------------------------------------------------------------------------

void HapticGuidance::update_trajectory(double time) {
    // compute trajectory
    for (int i = 0; i < 54; i++) {
        trajectory_y_data[i] = 540 - i * 20;
        trajectory_x_data[i] = (int)(300.0 * -sin(2.0 * mel::PI * sin_freq_ * (time + (double)i * 20.0 / 1080.0)) * 
                                              cos(2.0 * mel::PI * cos_freq_ * (time + (double)i * 20.0 / 1080.0)));
    }
    // send trajectory to Unity
    trajectory_x_.write(trajectory_x_data);
    trajectory_y_.write(trajectory_y_data);
}


void HapticGuidance::update_expert(double time) {
    double traj_point;
    std::array<double,540> check_array;
    double min = 1000;
    int pos_min = 0;
    double traj_point_min;
    for (int i = 540 - (int)length_; i < 540; i++) {
        traj_point = amplitude_ / 1000.0 * -sin(2.0*mel::PI*sin_freq_*(time + (double)i    / 1080.0  )) *cos(2.0*mel::PI*cos_freq_*(time          + (double)i / 1080.0    ));
        check_array[i] = abs(length_/1000.0 - sqrt(pow(traj_point, 2) + pow(((double)i / 1000.0) - (540.0 - length_)/1000, 2)));
        if (check_array[i] < min) {
            min = check_array[i];
            pos_min = i;
            traj_point_min = traj_point;
        }
    }
    expert_position_[0] =  (int)(1000 * traj_point_min);
    expert_position_[1] =  540 - pos_min;
    exp_pos.write(expert_position_);
}


double HapticGuidance::compute_trajectory_error(double joint_angle) {
    double correct_angle = asin((double)expert_position_[0] / length_);
    return (joint_angle - correct_angle);
}

void HapticGuidance::update_unity(bool background, bool pendulum, bool trajectory_region, bool trajectory_center, bool expert, bool radius, bool stars) {
    unity_data_ = { 0,0,0,0,0,0,0 };
    if (background)
        unity_data_[0] = 1;
    if (pendulum)
        unity_data_[1] = 1;
    if (trajectory_region)
        unity_data_[2] = 1;
    if (trajectory_center)
        unity_data_[3] = 1;
    if (expert)
        unity_data_[4] = 1;
    if (radius)
        unity_data_[5] = 1;
    if (stars)
        unity_data_[6] = 1;
    unity_.write(unity_data_);
}
