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

double OpenWrist::compute_gravity_compensation(mel::uint32 joint) {
    mel::double_vec q = get_joint_positions();
    if (joint == 0)
        return 1.3939*sin(q[0]) - 0.395038*cos(q[0]) + 0.351716*cos(q[0])*cos(q[1]) + 0.0826463*cos(q[0])*sin(q[1]) + 0.0397738*cos(q[2])*sin(q[0]) - 0.0929844*sin(q[0])*sin(q[2]) - 0.0929844*cos(q[0])*cos(q[2])*sin(q[1]) - 0.0397738*cos(q[0])*sin(q[1])*sin(q[2]);
    else if (joint == 1)
        return -8.50591e-18*sin(q[0])*(4.13496e16*sin(q[1]) - 9.71634e15*cos(q[1]) + 1.09317e16*cos(q[1])*cos(q[2]) + 4.67601e15*cos(q[1])*sin(q[2]));
    else if (joint == 2)
        return 0.0929844*cos(q[0])*cos(q[2]) + 0.0397738*cos(q[0])*sin(q[2]) + 0.0929844*sin(q[0])*sin(q[1])*sin(q[2]) - 0.0397738*cos(q[2])*sin(q[0])*sin(q[1]);
    else
        return 0.0;
}

double OpenWrist::compute_friction_compensation(mel::uint32 joint) {
    return params_.kin_friction_[joint] * tanh(10.0 * joints_[joint]->get_velocity());
}

std::array<double, 3> OpenWrist::compute_friction_compensation() {
    std::array<double, 3> friction_torques = { 0,0,0 };
    for (int joint = 0; joint < 3; joint++) {
        friction_torques[joint] = params_.kin_friction_[joint] * tanh(10.0 * joints_[joint]->get_velocity());
    }
    return friction_torques;
}

std::array<double,3> OpenWrist::compute_gravity_compensation() {
    mel::double_vec q = get_joint_positions();
    std::array<double, 3> gravity_torques = { 0,0,0 };
    gravity_torques[0] = 1.3939*sin(q[0]) - 0.395038*cos(q[0]) + 0.351716*cos(q[0])*cos(q[1]) + 0.0826463*cos(q[0])*sin(q[1]) + 0.0397738*cos(q[2])*sin(q[0]) - 0.0929844*sin(q[0])*sin(q[2]) - 0.0929844*cos(q[0])*cos(q[2])*sin(q[1]) - 0.0397738*cos(q[0])*sin(q[1])*sin(q[2]);
    gravity_torques[1] = -8.50591e-18*sin(q[0])*(4.13496e16*sin(q[1]) - 9.71634e15*cos(q[1]) + 1.09317e16*cos(q[1])*cos(q[2]) + 4.67601e15*cos(q[1])*sin(q[2]));
    gravity_torques[2] = 0.0929844*cos(q[0])*cos(q[2]) + 0.0397738*cos(q[0])*sin(q[2]) + 0.0929844*sin(q[0])*sin(q[1])*sin(q[2]) - 0.0397738*cos(q[2])*sin(q[0])*sin(q[1]);
    return gravity_torques;
}


