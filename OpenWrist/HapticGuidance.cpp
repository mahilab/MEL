#include "HapticGuidance.h"

HapticGuidance::HapticGuidance(mel::Clock& clock, mel::Daq* daq, OpenWrist& open_wrist, Cuff& cuff, GuiFlag& gui_flag, int input_mode,
    int subject_number, int condition, std::string start_trial):
    StateMachine(8), 
    clock_(clock),
    daq_(daq),
    open_wrist_(open_wrist), 
    cuff_(cuff),
    gui_flag_(gui_flag),
    INPUT_MODE_(input_mode),
    SUBJECT_NUMBER_(subject_number),
    CONDITION_(condition)
{
    if (subject_number < 10)
        DIRECTORY_ = "S0" + std::to_string(subject_number);
    else
        DIRECTORY_ = "S" + std::to_string(subject_number);

    build_experiment_();

    for (int i = 0; i < TRIALS_NAMES_.size(); ++i) {
        if (start_trial == TRIALS_NAMES_[i])
            current_trial_index_ = i-1;
    }

    perlin_module_.SetOctaveCount(1.0);
    perlin_module_.SetFrequency(1.0);
    perlin_module_.SetPersistence(0.1);
}

void HapticGuidance::wait_for_continue_input() {
    if (INPUT_MODE_ == 0) {
        getchar();
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


void HapticGuidance::build_experiment_() {
    for (auto it = BLOCK_ORDER_.begin(); it != BLOCK_ORDER_.end(); ++it) {
        NUM_BLOCKS_[*it] += 1;
        for (int i = 0; i < NUM_TRIALS_[*it]; i++) {
            TRIALS_ORDER_.push_back(*it);
            TRIALS_NAMES_.push_back(BLOCK_TAGS_[*it] + std::to_string(NUM_BLOCKS_[*it]) + "-" + std::to_string(i + 1));
        }
    }
}

HapticGuidance::BlockType HapticGuidance::get_next_trial() {

}

//-----------------------------------------------------------------------------
// START STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_start(const mel::NoEventData*) {
    
    // enable DAQ(s)
    if (CONDITION_ > 0) {
        mel::print("Waiting for user input to activate Daq <" + daq_->name_ + ">.");
        wait_for_continue_input();
        daq_->activate();
        allow_continue_input();
    }
    
    // enable and pretension CUFF
    if (CONDITION_ == 2 || CONDITION_ == 3) {
        std::cout << "Waiting for user input to pretension CUFF" << std::endl;
        wait_for_continue_input();
        cuff_.enable();
        cuff_.pretensioning(CUFF_NORMAL_FORCE_, offset, scaling_factor);
        allow_continue_input();
    } 
    
    event(ST_TRANSITION);   
}

//-----------------------------------------------------------------------------
// FAMILIARIZATION STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_familiarization(const mel::NoEventData*) {

    // create a new data log
    mel::DataLog log(TRIALS_NAMES_[current_trial_index_]);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // show/hide Unity elements
    // [ background, pendulum on/off , trajectory region on/off , trajectory center on/off, expert on/off, radius on/off , stars on/off ]
    unity_data_ = { 1, 1, 1, 0, 0, 0, 1 };
    unity_.write(unity_data_);

    // reset and start the hardware clock
    clock_.restart();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[FAMILIARIZATION] && !ctrl_c_ && !gui_flag_.check_flag(2)) {

        // read and reload DAQ
        daq_->reload_watchdog();
        daq_->read_all();

        // compute trajectory
        for (int i = 0; i < 54; i++) {
            trajectory_y_data[i] = 540 - i * 20;
            trajectory_x_data[i] = (int)(300.0 * -sin(2.0*mel::PI*0.1*(clock_.time() + (double)i * 20.0 / 1080.0)) *cos(2.0*mel::PI*0.2*(clock_.time() + (double)i * 20.0 / 1080.0)));
        }

        // sent trajectory to Unity
        trajectory_x_.write(trajectory_x_data);
        trajectory_y_.write(trajectory_y_data);

        // solve for expert position
        estimate_expert_position(exp_pos_data, clock_.time(), 0.3, 0.1, 0.2, 0.45, 450.0);
        exp_pos.write(exp_pos_data);

        // compute anglular error
        double error = find_error_angle(open_wrist_.joints_[0]->get_position(), exp_pos_data, 0.45);

        // step the pendulum simuation
        pendulum_.step_simulation(clock_.time(), open_wrist_.joints_[0]->get_position(), open_wrist_.joints_[0]->get_velocity());

        // set CUFF positions
        if (CONDITION_ == 2) {
            double noise = perlin_module_.GetValue(clock_.time(), 0.0, 0.0);
            cuff_.set_motor_positions((short int)(noise * CUFF_NOISE_GAIN_) + offset[0], (short int)(noise * CUFF_NOISE_GAIN_) + offset[1], true);
        }
        else if (CONDITION_ == 3)
            cuff_.set_motor_positions((short int)(-error * CUFF_GUIDANCE_GAIN_ + offset[0]), (short int)(-error * CUFF_GUIDANCE_GAIN_ + offset[1]), true);

        // set OpenWrist joint torques
        open_wrist_.joints_[0]->set_torque(open_wrist_.compute_gravity_compensation(0) + 0.75 * open_wrist_.compute_friction_compensation(0) - pendulum_.Tau[0]);
        open_wrist_.joints_[1]->set_torque(mel::pd_controller(40, 1.0, 0, open_wrist_.joints_[1]->get_position(), 0, open_wrist_.joints_[1]->get_velocity()));
        //ow_->joints_[2]->set_torque(ow_->compute_friction_compensation(2) * 0.5);

        // update OpenWrist state
        open_wrist_.update_state_map();

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // write the DAQ
        daq_->write_all();

        // wait for the next clock cycle
        clock_.wait();        
    }

    // save the log
    log.save_data(DIRECTORY_ + "\\FAMILIARIZATION");

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// EVALUATION STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_evaluation(const mel::NoEventData*) {
   
    // create a new data log
    mel::DataLog log(TRIALS_NAMES_[current_trial_index_]);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.restart();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[EVALUATION] && !ctrl_c_ && !gui_flag_.check_flag(2)) {

        // read and reload DAQ
        daq_->reload_watchdog();
        daq_->read_all();

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }


    // save the log
    log.save_data(DIRECTORY_ + "\\EVALUATION");

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// TRAINING STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_training(const mel::NoEventData*) {

    // create a new data log
    mel::DataLog log(TRIALS_NAMES_[current_trial_index_]);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.restart();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[TRAINING] && !ctrl_c_ && !gui_flag_.check_flag(2)) {

        // read and reload DAQ
        daq_->reload_watchdog();
        daq_->read_all();

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }

    // save the log
    log.save_data(DIRECTORY_ + "\\TRAINING");

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// BREAK STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_break(const mel::NoEventData*) {

    // create a new data log
    mel::DataLog log(TRIALS_NAMES_[current_trial_index_]);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.restart();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[BREAK] && !ctrl_c_ && !gui_flag_.check_flag(2)) {

        // read and reload DAQ
        daq_->reload_watchdog();
        daq_->read_all();

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }

    // save the log
    log.save_data(DIRECTORY_ + "\\BREAK");

    // transition to the next state
    event(ST_TRANSITION);
}

//-----------------------------------------------------------------------------
// GENERALIZATION STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_generalization(const mel::NoEventData*) {
    
    // create a new data log
    mel::DataLog log(TRIALS_NAMES_[current_trial_index_]);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.restart();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[EVALUATION] && !ctrl_c_ && !gui_flag_.check_flag(2)) {

        // read and reload DAQ
        daq_->reload_watchdog();
        daq_->read_all();

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }

    // save the log
    log.save_data(DIRECTORY_ + "\\GENERALIZAION");

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
        daq_->stop_watchdog();
    }

    // reallow input from GUI
    allow_continue_input();

    // increment the trial;
    current_trial_index_ += 1;

    if (current_trial_index_ < TRIALS_NAMES_.size() && !ctrl_c_) {
        mel::print("Next trial: <" + TRIALS_NAMES_[current_trial_index_] + ">. Waiting for user input to begin.");
        wait_for_continue_input();        

        // resume hardware
        if (CONDITION_ > 0) {
            open_wrist_.enable();
            daq_->start_watchdog(0.1);
        }

        mel::print("Starting trial <" + TRIALS_NAMES_[current_trial_index_] + ">. Press Ctrl+C to terminate the trial.");

        // transition to the next state
        if (TRIALS_ORDER_[current_trial_index_] == FAMILIARIZATION)
            event(ST_FAMILIARIZATION);
        else if (TRIALS_ORDER_[current_trial_index_] == EVALUATION)
            event(ST_EVALUATION);
        else if (TRIALS_ORDER_[current_trial_index_] == TRAINING)
            event(ST_TRAINING);
        else if (TRIALS_ORDER_[current_trial_index_] == BREAK)
            event(ST_BREAK);
        else if (TRIALS_ORDER_[current_trial_index_] == GENERALIZATION)
            event(ST_GENERALIZATION);
    }
    else {
        event(ST_STOP);
    }
}

//-----------------------------------------------------------------------------
// STOP STATE FUNCTION
//-----------------------------------------------------------------------------
void HapticGuidance::sf_stop(const mel::NoEventData*) {
    mel::print("\n All trials completed. Ending experiment and disabling hardware.");
    daq_->deactivate();
}

//-----------------------------------------------------------------------------
// UTILITY FUNCTIONS
//-----------------------------------------------------------------------------
void HapticGuidance::estimate_expert_position(std::array<int,2>& coordinates_pix, double time, double amplitude_sc_m, double freq_sine, double freq_cosine, double length_m, double joint_pos_y_pix) {
    double traj_point;
    std::array<double,540> check_array;
    double min = 1000;
    int pos_min = 0;
    double traj_point_min;
    for (int i = 540 - (int)joint_pos_y_pix; i < 540; i++) {
        traj_point = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + (double)i    / 1080.0  )) *cos(2.0*mel::PI*freq_cosine*(time          + (double)i / 1080.0    ));
        check_array[i] = abs(length_m - sqrt(pow(traj_point, 2) + pow(((double)i / 1000.0) - (540.0 - joint_pos_y_pix)/1000, 2)));
        if (check_array[i] < min) {
            min = check_array[i];
            pos_min = i;
            traj_point_min = traj_point;
        }
    }
    coordinates_pix[0] =  (int)(1000 * traj_point_min);
    coordinates_pix[1] =  540 - pos_min;
}


double HapticGuidance::find_error_angle(double actual_angle, std::array<int,2> intersection_pix, double length_m) {
    double correct_angle = asin((double)intersection_pix[0] / (length_m * 1000.0));
    return (actual_angle - correct_angle);
}

