#include "HapticGuidance.h"

HapticGuidance::HapticGuidance(mel::Clock& clock, OpenWrist* open_wrist, mel::Daq* daq, GuiFlag& gui_flag) : 
    StateMachine(clock, 3), 
    ow_(open_wrist), 
    daq_(daq), 
    gui_flag_(gui_flag)
{

}

void HapticGuidance::ST_Start(const mel::NoEventData*) {
    std::array<double, 5> flags_data;
    std::cout << "Waiting to activate Daq <" << daq_->name_ << ">." << std::endl;
    gui_flag_.wait_for_flag(1);
    daq_->activate();
    gui_flag_.reset_flag(0);
    std::cout << "Waiting to enable OpenWrist." << std::endl;
    gui_flag_.wait_for_flag(2);
    ow_->enable();
    daq_->zero_encoders();
    gui_flag_.reset_flag(0);
    //std::cout << "Waiting to enable CUFF" << std::endl;
    //gui_flag_.wait_for_flag(3);
    //cuff_.enable();
    //gui_flag_.reset_flag(0);
    //std::cout << "Waiting to pretension CUFF" << std::endl;
    //gui_flag_.wait_for_flag(4);
    //cuff_.pretensioning(2, offset, scaling_factor);
    //gui_flag_.reset_flag(0);
    std::cout << "Waiting to start the controller." << std::endl;
    gui_flag_.wait_for_flag(5);
    daq_->start_watchdog(0.1);
    gui_flag_.reset_flag(0);
    std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
    event(ST_STEP);
}

void HapticGuidance::ST_Step(const mel::NoEventData*) {
    // read and reload DAQ
    daq_->reload_watchdog();
    daq_->read_all();

    // compute trajectory
    for (int i = 0; i < 20; i++) {
        trajectory_data[i] = 0.3 * -sin(2 * mel::PI*.1*(clock_.time() + 1.0 / 20.0 * i)) * cos(2 * mel::PI*.2*(clock_.time() + 1.0 / 20.0 * i));
    }
    trajectory.write(trajectory_data);

    // solve for expert position
    dumb_solution(exp_pos_data, clock_.time(), 0.3, 0.1, 0.2, 0.45, 450.0);
    exp_pos.write(exp_pos_data);

    // step the pendulum simuation
    pendulum.step_simulation(clock_.time(), ow_->joints_[0]->get_position(), ow_->joints_[0]->get_velocity());

    // set OpenWrist joint torques
    ow_->joints_[0]->set_torque(ow_->compute_gravity_compensation(0) + 0.5*ow_->compute_friction_compensation(0) - pendulum.Tau[0]);
    ow_->joints_[1]->set_torque(ow_->compute_gravity_compensation(1) + 0.5*ow_->compute_friction_compensation(1));
    //ow_->joints_[2]->set_torque(ow_->compute_friction_compensation(2) * 0.5);

    // update OpenWrist state
    ow_->update_state_map();

    // write the DAQ
    daq_->write_all();

    // check condition to stop step function
    if (gui_flag_.check_flag(6))
        event(ST_STOP);
}

void HapticGuidance::ST_Stop(const mel::NoEventData*) {
    ow_->disable();
    cuff_.disable();
    daq_->deactivate();
    stop_ = true;
}

void HapticGuidance::ctrl_c_task() {
    ow_->disable();
    cuff_.disable();
    daq_->deactivate();
}

void HapticGuidance::dumb_solution(std::array<int,2>& coordinates_pix, double time, double amplitude_sc_m, double freq_sine, double freq_cosine, double length_m, double joint_pos_y_pix) {
    double traj_point;
    std::array<double,540> check_array;
    double min = 1000;
    int pos_min = 0;
    double traj_point_min;
    for (int i = 540 - (int)joint_pos_y_pix; i < 540; i++) {
        //                 0.3      * -sin(2  *mel::PI*    .1   *(clock_.time() + 1.0 / 20.0 * i)) *cos(2  *mel::PI*     .2    *(clock_.time() + 1.0 / 20.0 * i));
        traj_point = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + (double)i / 1080.0  )) *cos(2.0*mel::PI*freq_cosine*(time + (double)i / 1080.0    ));
        check_array[i] = abs(length_m - sqrt(pow(traj_point, 2) + pow(((double)i / 1000.0) - (540.0 - joint_pos_y_pix), 2)));
        mel::print(check_array[i]);
        if (check_array[i] < min) {
            min = check_array[i];
            pos_min = i;
            traj_point_min = traj_point;
        }
    }
    //mel::print(pos_min);
    coordinates_pix[0] =  (int)(1000 * traj_point_min);
    coordinates_pix[1] =  540 - pos_min;
}


double find_error_angle(double actual_angle, int* intersection_pix, double length_m) {
    double correct_angle = asin(intersection_pix[0] / (length_m * 1000));
    return (actual_angle - correct_angle);
}