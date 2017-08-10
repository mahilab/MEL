#include "HapticGuidance.h"

HapticGuidance::HapticGuidance(mel::Clock& clock, OpenWrist* open_wrist, mel::Daq* daq, GuiFlag& gui_flag, int input_mode,
    int subject_number, int condition, int task, int task_block, int trial_num):
    StateMachine(7), 
    clock_(clock),
    ow_(open_wrist), 
    daq_(daq), 
    gui_flag_(gui_flag),
    INPUT_MODE_(input_mode),
    SUBJECT_NUMBER_(subject_number),
    CONDITION_(condition),
    current_task_((Blocks)task),
    current_task_block_(task_block),
    current_trial_num_(trial_num)
{
    if (subject_number < 10)
        DIRECTORY_ = "S0" + std::to_string(subject_number);
    else
        DIRECTORY_ = "S" + std::to_string(subject_number);

    generate_experiment_states();
    mel::print(experiment_states_.size());
    for (int i = 0; i < experiment_states_.size(); i++)
        mel::print(experiment_states_[i]);

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

HapticGuidance::States HapticGuidance::get_start_state() {
    if (current_task_ > 0) {
        block_counter_[FAMILIARIZATION] = 1;
        if (current_task_block_ > BREAK_AFTER_TRAINING_BLOCK_) {
            block_counter_[BREAK] = 1;
        }      
        if (current_task_ == EVALUATION) {
            block_counter_[EVALUATION] = current_task_block_;
            block_counter_[TRAINING] = current_task_block_;
            return ST_EVALUATION;
        }
        else if (current_task_ == TRAINING) {
            block_counter_[EVALUATION] = current_task_block_ + 1;
            block_counter_[TRAINING] = current_task_block_;
            return ST_TRAINING;
        }
        else if (current_task_ == BREAK) {
            block_counter_[BREAK] = 0;
            block_counter_[EVALUATION] = BREAK_AFTER_TRAINING_BLOCK_ + 1;
            block_counter_[TRAINING] = BREAK_AFTER_TRAINING_BLOCK_ + 1;
            return ST_BREAK;
        }
        else if (current_task_ == GENERALIZATION) {
            block_counter_[BREAK] = 1;
            block_counter_[EVALUATION] = NUM_BLOCKS_[EVALUATION];
            block_counter_[TRAINING] = NUM_BLOCKS_[TRAINING];
            return ST_GENERALIZATION;
        }         
    }
    else {
        return ST_FAMILIARIZATION;
    }
}

void HapticGuidance::generate_experiment_states() {
    for (auto it = BLOCK_ORDER_.begin(); it != BLOCK_ORDER_.end(); ++it) {
        States new_state;
        if (*it == FAMILIARIZATION)
            new_state = ST_FAMILIARIZATION;
        else if (*it == EVALUATION)
            new_state = ST_EVALUATION;
        else if (*it == TRAINING)
            new_state = ST_TRAINING;
        else if (*it == BREAK)
            new_state = ST_BREAK;
        else if (*it == GENERALIZATION)
            new_state = ST_GENERALIZATION;
        for (int i = 0; i < NUM_TRIALS_[*it]; i++) {
            experiment_states_.push_back(new_state);
        }
    }
    experiment_states_.push_back(ST_STOP);
}


HapticGuidance::States HapticGuidance::get_next_state() {
    current_trial_num_ += 1;
    if (current_trial_num_ == NUM_TRIALS_[current_task_])
    {
        current_trial_num_ = 0;
        block_counter_[current_task_] += 1; 
        if (current_task_ == FAMILIARIZATION) {
            current_task_ = EVALUATION;
            return ST_EVALUATION;
        }
        else if (current_task_ == EVALUATION) {
            if (block_counter_[EVALUATION] == NUM_BLOCKS_[EVALUATION]) {
                current_task_ = GENERALIZATION;
                return ST_GENERALIZATION;
            }
            else {
                current_task_ = TRAINING;
                return ST_TRAINING;
            }
        }
        else if (current_task_ == TRAINING) {
            if (block_counter_[TRAINING] == BREAK_AFTER_TRAINING_BLOCK_ + 1) {
                current_task_ = BREAK;
                return ST_BREAK;
            }
            else {
                current_task_ = EVALUATION;
                return ST_EVALUATION;
            }
        }    
        else if (current_task_ == BREAK) {
            current_task_ = EVALUATION;
            return ST_EVALUATION;
        }
        else if (current_task_ == GENERALIZATION)
        {
            return ST_STOP;
        }
    }
    else {
        if (current_task_ == FAMILIARIZATION)
            return ST_EVALUATION;
        else if (current_task_ == EVALUATION)
            return ST_EVALUATION;
        else if (current_task_ == TRAINING)
            return ST_TRAINING;
        else if (current_task_ == GENERALIZATION)
            return ST_GENERALIZATION;
    }
}

void HapticGuidance::sf_init(const mel::NoEventData*) {
    
    // Enable DAQ and OpenWrist
    std::cout << "Waiting for user input to activate Daq <" << daq_->name_ << ">." << std::endl;
    wait_for_continue_input();
    daq_->activate();
    allow_continue_input();
    std::cout << "Waiting for user input to enable OpenWrist." << std::endl;
    wait_for_continue_input();
    ow_->enable();
    allow_continue_input();

    if (CONDITION_ == 0) {
        mel::print("Waiting for user input to skip enabling guidance device");
        wait_for_continue_input();
        allow_continue_input();
    }
    else if (CONDITION_ == 1 || CONDITION_ == 2) {
        std::cout << "Waiting for user input to enable and pretension CUFF" << std::endl;
        wait_for_continue_input();
        cuff_.enable();
        cuff_.pretensioning(3, offset, scaling_factor);
        allow_continue_input();
    } 
    else if (CONDITION_ == 3) {
        std::cout << "Waiting user input to enable MahiExo-II" << std::endl;
        wait_for_continue_input();
        // TODO: Enable MEII
        allow_continue_input();
    }
    
    std::cout << "Waiting for user input to start the experiment." << std::endl;
    wait_for_continue_input();
    //allow_continue_input();
    std::cout << "Starting the experiment. Press CTRL+C to stop." << std::endl;
    event(get_start_state());
   
}

void HapticGuidance::sf_familiarization(const mel::NoEventData*) {

    // generate trial name and display in command
    std::string trial_name = "familiarization_" + std::to_string(block_counter_[FAMILIARIZATION] + 1) + "_" + std::to_string(current_trial_num_ + 1);
    mel::print("Running <" + trial_name + ">");

    // create a new data log
    mel::DataLog log(trial_name);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.reset();
    clock_.start();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[FAMILIARIZATION] && !ctrl_c_ && !stop_) {

        // read and reload DAQ
        daq_->reload_watchdog();
        daq_->read_all();

        // compute trajectory
        for (int i = 0; i < 54; i++) {
            trajectory_y_data[i] = 540 - i * 20;
            trajectory_x_data[i] = (int)(300.0 * -sin(2.0*mel::PI*0.1*(clock_.time() + (double)i * 20.0 / 1080.0)) *cos(2.0*mel::PI*0.2*(clock_.time() + (double)i * 20.0 / 1080.0)));
        }

        // sent trajectory to Unity
        trajectory_x.write(trajectory_x_data);
        trajectory_y.write(trajectory_y_data);

        // solve for expert position
        estimate_expert_position(exp_pos_data, clock_.time(), 0.3, 0.1, 0.2, 0.45, 450.0);
        exp_pos.write(exp_pos_data);

        // compute anglular error
        double error = find_error_angle(ow_->joints_[0]->get_position(), exp_pos_data, 0.45);

        // step the pendulum simuation
        pendulum.step_simulation(clock_.time(), ow_->joints_[0]->get_position(), ow_->joints_[0]->get_velocity());

        // set CUFF positions
        if (CONDITION_ == 1) {
            double noise = perlin_module_.GetValue(clock_.time(), 0.0, 0.0);
            cuff_.set_motor_positions((short int)(noise * 8400.0) + offset[0], (short int)(noise * 8400.0) + offset[1], true);
        }
        else if (CONDITION_ == 2)
            cuff_.set_motor_positions((short int)(-error * 20000 + offset[0]), (short int)(-error * 20000 + offset[1]), true);


        // set OpenWrist joint torques
        ow_->joints_[0]->set_torque(ow_->compute_gravity_compensation(0) + 0.5*ow_->compute_friction_compensation(0) - pendulum.Tau[0]);
        ow_->joints_[1]->set_torque(ow_->compute_gravity_compensation(1) + 0.5*ow_->compute_friction_compensation(1));
        //ow_->joints_[2]->set_torque(ow_->compute_friction_compensation(2) * 0.5);

        // update OpenWrist state
        ow_->update_state_map();

        // write the DAQ
        daq_->write_all();

        // check for terminate signal
        if (gui_flag_.check_flag(2)) {
            stop_ = true;
            break;
        }

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();        
    }

    // stop the clock
    clock_.stop();
    // save the log
    log.save_data(DIRECTORY_ + "\\familiariztion");
    mel::print("Completed <" + trial_name + ">. Waiting for user input to continue.");
    // wait for user input to continue
    allow_continue_input();
    wait_for_continue_input();
    // transition to the next state
    event(get_next_state());

}

void HapticGuidance::sf_evaluation(const mel::NoEventData*) {
   
    // generate trial name and display in command
    std::string trial_name = "evaluation_" + std::to_string(block_counter_[EVALUATION] + 1) + "_" + std::to_string(current_trial_num_ + 1);
    mel::print("Running <" + trial_name + ">");

    // create a new data log
    mel::DataLog log(trial_name);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.reset();
    clock_.start();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[EVALUATION] && !ctrl_c_ && !stop_) {

        // check for terminate signal
        if (gui_flag_.check_flag(2)) {
            stop_ = true;
            break;
        }

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }

    // stop the clock
    clock_.stop();
    // save the log
    log.save_data(DIRECTORY_ + "\\evaluation");

    mel::print("Completed <" + trial_name + ">. Waiting for user input to continue.");
    // wait for user input to continue
    allow_continue_input();
    wait_for_continue_input();
    // transition to the next state
    event(get_next_state());

}

void HapticGuidance::sf_training(const mel::NoEventData*) {
    // generate trial name and display in command
    std::string trial_name = "training_" + std::to_string(block_counter_[TRAINING] + 1) + "_" + std::to_string(current_trial_num_ + 1);
    mel::print("Running <" + trial_name + ">");

    // create a new data log
    mel::DataLog log(trial_name);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.reset();
    clock_.start();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[TRAINING] && !ctrl_c_ && !stop_) {

        // check for terminate signal
        if (gui_flag_.check_flag(2)) {
            stop_ = true;
            break;
        }

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }

    // stop the clock
    clock_.stop();
    // save the log
    log.save_data(DIRECTORY_ + "\\training");

    mel::print("Completed <" + trial_name + ">. Waiting for user input to continue.");
    // wait for user input to continue
    allow_continue_input();
    wait_for_continue_input();
    // transition to the next state
    event(get_next_state());

}


void HapticGuidance::sf_break(const mel::NoEventData*) {

    // generate trial name and display in command
    std::string trial_name = "break_" + std::to_string(block_counter_[BREAK] + 1) + "_" + std::to_string(current_trial_num_ + 1);
    mel::print("Running <" + trial_name + ">");

    // create a new data log
    mel::DataLog log(trial_name);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.reset();
    clock_.start();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[BREAK] && !ctrl_c_ && !stop_) {

        // check for terminate signal
        if (gui_flag_.check_flag(2)) {
            stop_ = true;
            break;
        }

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }

    // stop the clock
    clock_.stop();
    // save the log
    log.save_data(DIRECTORY_ + "\\break");

    mel::print("Completed <" + trial_name + ">. Waiting for user input to continue.");
    // wait for user input to continue
    allow_continue_input();
    wait_for_continue_input();
    // transition to the next state
    event(get_next_state());


}

void HapticGuidance::sf_generalization(const mel::NoEventData*) {
    
    // generate trial name and display in command
    std::string trial_name = "generalization_" + std::to_string(block_counter_[GENERALIZATION] + 1) + "_" + std::to_string(current_trial_num_ + 1);
    mel::print("Running <" + trial_name + ">");

    // create a new data log
    mel::DataLog log(trial_name);
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.reset();
    clock_.start();

    // enter the control loop
    while (clock_.time() < LENGTH_TRIALS_[EVALUATION] && !ctrl_c_ && !stop_) {

        // check for terminate signal
        if (gui_flag_.check_flag(2)) {
            stop_ = true;
            break;
        }

        // log data
        log_data = { clock_.time() };
        log.add_row(log_data);

        // wait for the next clock cycle
        clock_.wait();
    }

    // stop the clock
    clock_.stop();
    // save the log
    log.save_data(DIRECTORY_ + "\\generalization");

    mel::print("Completed <" + trial_name + ">. Waiting for user input to continue.");
    // wait for user input to continue
    allow_continue_input();
    wait_for_continue_input();
    // transition to the next state
    event(get_next_state());


}

void HapticGuidance::sf_stop(const mel::NoEventData*) {
    mel::print("All trials completed. Ending experiment");
    ow_->disable();
    cuff_.disable();
    daq_->deactivate();
    stop_ = true;
}

void HapticGuidance::ctrl_c_task() {
    mel::print("Ctrl+C pressed. Terminating experiment.");
    ow_->disable();
    cuff_.disable();
    daq_->deactivate();
    stop_ = true;
}

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

