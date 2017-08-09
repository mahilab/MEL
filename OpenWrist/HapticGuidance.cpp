#include "HapticGuidance.h"

HapticGuidance::HapticGuidance(mel::Clock& clock, OpenWrist* open_wrist, mel::Daq* daq, GuiFlag& gui_flag, int subject_number, int condition, int input_mode) : 
    StateMachine(7), 
    clock_(clock),
    ow_(open_wrist), 
    daq_(daq), 
    gui_flag_(gui_flag),
    SUBJECT_NUMBER_(subject_number),
    CONDITION_(condition),
    INPUT_MODE_(input_mode)
{
    if (subject_number < 10)
        DIRECTORY_ = "S0" + std::to_string(subject_number);
    else
        DIRECTORY_ = "S" + std::to_string(subject_number);
}

void HapticGuidance::wait_for_continue_input() {
    if (INPUT_MODE_ == 0) {
        getchar();
    }
    else if (INPUT_MODE_ = 1) {
        gui_flag_.wait_for_flag(1);
    }
}

void HapticGuidance::allow_continue_input() {
    if (INPUT_MODE_ == 1)
        gui_flag_.reset_flag(0);
}

void HapticGuidance::sf_init(const mel::NoEventData*) {
    
    // Enable DAQ and OpenWrist
    std::cout << "Waiting to activate Daq <" << daq_->name_ << ">." << std::endl;
    wait_for_continue_input();
    daq_->activate();
    allow_continue_input();
    std::cout << "Waiting to enable OpenWrist." << std::endl;
    wait_for_continue_input();
    ow_->enable();
    allow_continue_input();

    if (CONDITION_ == 1 || CONDITION_ == 2) {
        std::cout << "Waiting to enable CUFF" << std::endl;
        wait_for_continue_input();
        cuff_.enable();
        allow_continue_input();
        std::cout << "Waiting to pretension CUFF" << std::endl;
        wait_for_continue_input();
        cuff_.pretensioning(4, offset, scaling_factor);
        allow_continue_input();
    }

    if (CONDITION_ == 3) {

    }
    
    std::cout << "Waiting to start the controller." << std::endl;
    wait_for_continue_input();
    allow_continue_input();
    std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
    event(ST_FAMILIARIZATION);
}

void HapticGuidance::sf_familiarization(const mel::NoEventData*) {

    mel::print("Running <Familiarization>.");

    // create a new data log
    mel::DataLog log("familiarization");
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.reset();
    clock_.start();

    while (clock_.time() < FAMILIARIZATION_LENGTH && !ctrl_c_ && !stop_) {

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
        if (gui_flag_.check_flag(2))
            break;

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
    mel::print("Completed <Familiarization>. Waiting for user input to continue.");
    // wait for user input to continue
    wait_for_continue_input();
    allow_continue_input();
    // transition to the next state
    event(ST_EVALUATION);
}

void HapticGuidance::sf_evaluation(const mel::NoEventData*) {
   
    mel::print("Starting <Evaluation>");

    // create a new data log
    mel::DataLog log("evaluation_");
    log.add_col("Time [s]");
    std::vector<double> log_data = std::vector<double>(1, 0);

    // reset and start the hardware clock
    clock_.reset();
    clock_.start();

    while (clock_.time() < EVALUATION_LENGTH && !ctrl_c_ && !stop_) {

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

    mel::print("Completed <Evaluation>. Waiting for user input to continue.");
    // wait for user input to continue
    wait_for_continue_input();
    allow_continue_input();
    // transition to the next state
    event(ST_STOP);
}

void HapticGuidance::sf_training(const mel::NoEventData*) {

}


void HapticGuidance::sf_break(const mel::NoEventData*) {

}

void HapticGuidance::sf_generalization(const mel::NoEventData*) {

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