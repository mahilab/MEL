#include "HGCondition0.h"

void HGCondition0::start() {
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
}

void HGCondition0::step() {
    // read and reload DAQ
    daq_->reload_watchdog();
    daq_->read_all();

    // compute trajectory
    for (int i = 0; i < 20; i++) {
        trajectory_data[i] = 0.3 * -sin(2 * mel::PI*.1*(time() + 1.0 / 20.0 * i)) * cos(2 * mel::PI*.2*(time() + 1.0 / 20.0 * i));
    }
    trajectory.write(trajectory_data);

    // solve for expert position
    bisection_solution(exp_pos_data, time(), 0.3, 0.1, 0.2, 0.45, 450.0);
    exp_pos.write(exp_pos_data);

    // step the pendulum simuation
    pendulum.step_simulation(time(), ow_->joints_[0]->get_position(), ow_->joints_[0]->get_velocity());

    // set OpenWrist joint torques
    ow_->joints_[0]->set_torque(ow_->compute_gravity_compensation(0) + 0.5*ow_->compute_friction_compensation(0) - pendulum.Tau[0]);
    ow_->joints_[1]->set_torque(ow_->compute_gravity_compensation(1) + 0.5*ow_->compute_friction_compensation(1));
    //ow_->joints_[2]->set_torque(ow_->compute_friction_compensation(2) * 0.5);
    
    // update OpenWrist state
    ow_->update_state_map();
    
    // write the DAQ
    daq_->write_all();
}

void HGCondition0::stop() {
    ow_->disable();
    cuff_.disable();
    daq_->deactivate();
}

void HGCondition0::bisection_solution(std::array<int,2>& coordinates_pix, double time, double amplitude_sc_m, double freq_sine, double freq_cosine, double length_m, double joint_pos_y_pix) {

    double joint_pos_y_m = (540 - joint_pos_y_pix) / 1000;
    double pos_hig = 540 - joint_pos_y_pix;
    double pos_low = (length_m + joint_pos_y_m) * 1000 -1;
    double pos_mid = round((pos_low - pos_hig) / 2.0);
    //                                0.3      * -sin(2  *mel::PI*   .1    *(time()+ 1.0  / 20.0 * i))*cos(2  *mel::PI*     .2    *(time()+ 1.0 / 20.0 * i ));
    double difference_pos_hig = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_hig / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_hig / 1080.0)) - sqrt(pow(length_m, 2) - pow(pos_hig / 1000.0 - joint_pos_y_m, 2));
    double difference_neg_hig = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_hig / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_hig / 1080.0)) + sqrt(pow(length_m, 2) - pow(pos_hig / 1000.0 - joint_pos_y_m, 2));
    double difference_pos_low = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_low / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_low / 1080.0)) - sqrt(pow(length_m, 2) - pow(pos_low / 1000.0 - joint_pos_y_m, 2));
    double difference_neg_low = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_low / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_low / 1080.0)) + sqrt(pow(length_m, 2) - pow(pos_low / 1000.0 - joint_pos_y_m, 2));
    double difference_pos_mid = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_mid / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_mid / 1080.0)) - sqrt(pow(length_m, 2) - pow(pos_mid / 1000.0 - joint_pos_y_m, 2));
    double difference_neg_mid = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_mid / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_mid / 1080.0)) + sqrt(pow(length_m, 2) - pow(pos_mid / 1000.0 - joint_pos_y_m, 2));

    while ((pos_low - pos_hig) > 2.0) {
        std::cout << pos_hig << " " << pos_mid << " " << pos_low << std::endl;

        if (((difference_pos_hig * difference_pos_mid) < 0) || ((difference_neg_hig * difference_neg_mid) < 0)) {
            pos_low = pos_mid;
            difference_pos_low = difference_pos_mid;
            difference_neg_low = difference_neg_mid;
        }
        else {
            if (((difference_pos_low * difference_pos_mid) < 0) || ((difference_neg_low * difference_neg_mid) < 0)) {
                pos_hig = pos_mid;
                difference_pos_hig = difference_pos_mid;
                difference_neg_hig = difference_neg_mid;
            }
            else {
                std::cout << "\n Houston, we've got a problem";
            }
        }
        pos_mid = round((pos_low - pos_hig) / 2.0);
        difference_pos_mid = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_mid / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_mid / 1080.0)) - sqrt(pow(length_m, 2) - pow(pos_mid / 1000.0 - joint_pos_y_m, 2));
        difference_neg_mid = amplitude_sc_m * -sin(2.0*mel::PI*freq_sine*(time + pos_mid / 1080.0))*cos(2.0*mel::PI*freq_cosine*(time + pos_mid / 1080.0)) + sqrt(pow(length_m, 2) - pow(pos_mid / 1000.0 - joint_pos_y_m, 2));
    }

    coordinates_pix[1] = 540 - (int)pos_mid;
    if (difference_pos_mid < 1.0) {
        coordinates_pix[0] = (int)(sqrt(pow(length_m, 2) - pow(pos_mid / 1000.0 - joint_pos_y_m, 2)) * 1000);
    }
    else {
        if (difference_neg_mid < 1.0) {
            coordinates_pix[0] = (int)(-sqrt(pow(length_m, 2) - pow(pos_mid / 1000.0 - joint_pos_y_m, 2)) * 1000);
        }
        else {
            std::cout << "\n Error Occurred";
        }
    }

    //std::cout << time << " ";
    //mel::print(coordinates_pix);
}

double HGCondition0::find_error_angle(double actual_angle, int* intersection_pix, double length_m) {
    double correct_angle = asin(intersection_pix[0] / (length_m * 1000));

    return (actual_angle - correct_angle);
}