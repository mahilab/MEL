#include "OpenWrist.h"

OpenWrist::OpenWrist(Config configuration, Params parameters) :
    Exo("open_wrist"),
    config_(configuration),
    params_(parameters)
{ 
    for (int i = 0; i < 3; i++) {

        std::string num = std::to_string(i);

        mel::PositionSensor* encoder = new mel::Encoder("encoder_" + num, 
            params_.encoder_res_[i] / (2 * mel::PI), 
            config_.encoder_[i], 
            config_.encrate_[i]);

        position_sensors_.push_back(encoder);

        mel::Actuator* motor = new mel::Motor("motor_" + num,
            params_.kt_[i],
            params_.current_limits_[i],
            config_.amp_gains_[i],
            config_.command_[i],
            config_.enable_[i],
            mel::Actuator::EnableMode::High);

        actuators_.push_back(motor);

        mel::Joint* joint = new mel::Joint("joint_" + num,
            encoder,
            params_.eta_[i],
            motor,
            params_.eta_[i]);

        joints_.push_back(joint);        
    }
}


void OpenWrist::update_state_map(float timestamp) {
    state_[0] = timestamp;
    state_[1] = joints_[0]->get_position();
    state_[2] = joints_[1]->get_position();
    state_[3] = joints_[2]->get_position();
    state_[4] = joints_[0]->get_velocity();
    state_[5] = joints_[1]->get_velocity();
    state_[6] = joints_[2]->get_velocity();
    state_[7] = joints_[0]->get_torque();
    state_[8] = joints_[1]->get_torque();
    state_[9] = joints_[2]->get_torque();
    state_map_.write(state_);
}
