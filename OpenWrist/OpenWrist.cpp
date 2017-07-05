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
