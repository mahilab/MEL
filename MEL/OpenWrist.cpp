#include "OpenWrist.h"

namespace mel {

    OpenWrist::OpenWrist(Config configuration, Params parameters) :
        Exo("open_wrist"),
        config_(configuration),
        params_(parameters)
    {
        // for every joint
        for (int i = 0; i < 3; i++) {
            std::string num = std::to_string(i);

            // construct encoders
            PositionSensor* encoder = new Encoder("encoder_" + num,
                params_.encoder_res_[i] / (2 * PI),
                config_.encoder_[i],
                config_.encrate_[i]);

            position_sensors_.push_back(encoder);

            // construct motors
            Actuator* motor = new Motor("motor_" + num,
                params_.kt_[i],
                config_.amp_gains_[i],
                config_.command_[i],
                config_.enable_[i],
                Actuator::EnableMode::High,
                config_.sense_[i],
                params_.motor_current_limits_[i],
                params_.motor_torque_limits_[i],
                true);

            actuators_.push_back(motor);

            // construct joints
            Joint* joint = new Joint("joint_" + num,
                encoder,
                params_.eta_[i],
                motor,
                params_.eta_[i],
                std::array<double, 2>({ params_.pos_limits_neg_[i] , params_.pos_limits_pos_[i] }),
                params_.vel_limits_[i],
                params_.joint_torque_limits[i],
                true);

            joints_.push_back(joint);

            // add DAQs in config_ struct to daqs_ vector
            add_daq(config_.enable_[i].daq_);
            add_daq(config_.command_[i].daq_);
            add_daq(config_.sense_[i].daq_);
            add_daq(config_.encoder_[i].daq_);
            add_daq(config_.encrate_[i].daq_);
        }
    }

    OpenWrist::~OpenWrist() {
        if (enabled_)
            disable();
    }

    void OpenWrist::update_state_map() {
        state_[0] = joints_[0]->get_position();
        state_[1] = joints_[1]->get_position();
        state_[2] = joints_[2]->get_position();
        state_[3] = joints_[0]->get_velocity();
        state_[4] = joints_[1]->get_velocity();
        state_[5] = joints_[2]->get_velocity();
        state_[6] = joints_[0]->get_torque();
        state_[7] = joints_[1]->get_torque();
        state_[8] = joints_[2]->get_torque();
        state_map_.write(state_);
    }

    void OpenWrist::add_daq(Daq* daq) {
        for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
            if ((*it)->name_ == daq->name_)
                return;
        }
        daqs_.push_back(daq);
    }

    double OpenWrist::compute_gravity_compensation(uint32 joint) {
        double_vec q = get_joint_positions();
        if (joint == 0)
            return 1.3939*sin(q[0]) - 0.395038*cos(q[0]) + 0.351716*cos(q[0])*cos(q[1]) + 0.0826463*cos(q[0])*sin(q[1]) + 0.0397738*cos(q[2])*sin(q[0]) - 0.0929844*sin(q[0])*sin(q[2]) - 0.0929844*cos(q[0])*cos(q[2])*sin(q[1]) - 0.0397738*cos(q[0])*sin(q[1])*sin(q[2]);
        else if (joint == 1)
            return -8.50591e-18*sin(q[0])*(4.13496e16*sin(q[1]) - 9.71634e15*cos(q[1]) + 1.09317e16*cos(q[1])*cos(q[2]) + 4.67601e15*cos(q[1])*sin(q[2]));
        else if (joint == 2)
            return 0.0929844*cos(q[0])*cos(q[2]) + 0.0397738*cos(q[0])*sin(q[2]) + 0.0929844*sin(q[0])*sin(q[1])*sin(q[2]) - 0.0397738*cos(q[2])*sin(q[0])*sin(q[1]);
        else
            return 0.0;
    }

    double OpenWrist::compute_friction_compensation(uint32 joint) {
        return params_.kin_friction_[joint] * tanh(10.0 * joints_[joint]->get_velocity());
    }

    std::array<double, 3> OpenWrist::compute_friction_compensation() {
        std::array<double, 3> friction_torques = { 0,0,0 };
        for (int joint = 0; joint < 3; joint++) {
            friction_torques[joint] = params_.kin_friction_[joint] * tanh(10.0 * joints_[joint]->get_velocity());
        }
        return friction_torques;
    }

    std::array<double, 3> OpenWrist::compute_gravity_compensation() {
        double_vec q = get_joint_positions();
        std::array<double, 3> gravity_torques = { 0,0,0 };
        gravity_torques[0] = 1.3939*sin(q[0]) - 0.395038*cos(q[0]) + 0.351716*cos(q[0])*cos(q[1]) + 0.0826463*cos(q[0])*sin(q[1]) + 0.0397738*cos(q[2])*sin(q[0]) - 0.0929844*sin(q[0])*sin(q[2]) - 0.0929844*cos(q[0])*cos(q[2])*sin(q[1]) - 0.0397738*cos(q[0])*sin(q[1])*sin(q[2]);
        gravity_torques[1] = -8.50591e-18*sin(q[0])*(4.13496e16*sin(q[1]) - 9.71634e15*cos(q[1]) + 1.09317e16*cos(q[1])*cos(q[2]) + 4.67601e15*cos(q[1])*sin(q[2]));
        gravity_torques[2] = 0.0929844*cos(q[0])*cos(q[2]) + 0.0397738*cos(q[0])*sin(q[2]) + 0.0929844*sin(q[0])*sin(q[1])*sin(q[2]) - 0.0397738*cos(q[2])*sin(q[0])*sin(q[1]);
        return gravity_torques;
    }

    void OpenWrist::calibrate(mel::Daq* daq) {
        // make a new Clock and Controller
        Clock clock(1000);
        Controller controller(clock);
        // queue Tasks for the Controller to execute
        Task* task = new Calibration(this, daq);
        controller.queue_task(task);
        // execute the controller
        controller.execute();
        // clean up
        delete task;
    }

    void OpenWrist::Calibration::start() {
        std::cout << "Press ENTER to activate Daq <" << daq->name_ << ">.";
        getchar();
        daq->enable();
        daq->zero_encoders();
        std::cout << "Press ENTER to enable OpenWrist.";
        getchar();
        ow->enable();
        std::cout << "Press ENTER to start the calibration.";
        getchar();
        stored_positions.reserve(100000);
        daq->start_watchdog(0.1);
        std::cout << "Calibrating. Press CTRL+C to stop." << std::endl;
    }

    void OpenWrist::Calibration::step() {
        daq->read_all();
        daq->reload_watchdog();

        // iterate over all joints
        for (int i = 0; i < 3; i++) {

            // get positions and velocities
            double pos_act = ow->joints_[i]->get_position();
            double vel_act = ow->joints_[i]->get_velocity();

            double torque = 0;
            if (i == calibrating_joint) {

                if (!returning) {

                    // calculate torque req'd to move the calibrating joint forward at constant speed
                    pos_ref += dir[i] * vel_ref * delta_time();
                    torque = mel::pd_controller(kp_gains[i], kd_gains[i], pos_ref, pos_act, 0, vel_act);
                    torque = mel::saturate(torque, sat_torques[i]);

                    // check if the calibrating joint is still moving
                    stored_positions.push_back(pos_act);
                    bool moving = true;
                    if (stored_positions.size() > 500) {
                        moving = false;
                        for (int i = stored_positions.size() - 500; i < stored_positions.size(); i++) {
                            moving = stored_positions[i] != stored_positions[i - 1];
                            if (moving)
                                break;
                        }
                    }

                    // if it's not moving, it's at a hardstop so record the position and deduce the zero location
                    if (!moving) {
                        std::cout << "Joint <" << ow->joints_[i]->name_ << "> reached the reference position. Returning to zero ... ";
                        if (dir[i] > 0)
                            zeros[i] = pos_act - ow->params_.pos_limits_pos_[i];
                        else if (dir[i] < 0)
                            zeros[i] = pos_act - ow->params_.pos_limits_neg_[i];
                        returning = true;
                    }
                }

                else {
                    // calculate torque req'd to retur the calibrating joint back to zero
                    pos_ref -= dir[i] * vel_ref * delta_time();
                    torque = mel::pd_controller(kp_gains[i], kd_gains[i], pos_ref, pos_act, 0, vel_act);
                    torque = mel::saturate(torque, sat_torques[i]);

                    if (dir[i] * pos_ref <= dir[i] * zeros[i]) {
                        // reset for the next joint
                        calibrating_joint += 1;
                        pos_ref = 0;
                        returning = false;
                        std::cout << "Done" << std::endl;
                        if (calibrating_joint == 3)
                            std::cout << "All Joints are in their calibrated positions. Press CTRL+C to confirm the calibration." << std::endl;
                    }
                }

            }
            else {
                // lock all other joints at their zero positions
                torque = mel::pd_controller(kp_gains[i], kd_gains[i], zeros[i], pos_act, 0, vel_act);
            }
            ow->joints_[i]->set_torque(torque);
        }

        ow->update_state_map();
        daq->write_all();
    }

    void OpenWrist::Calibration::stop() {
        ow->disable();
        daq->zero_encoders();
        daq->disable();
    }

    void OpenWrist::transparency_mode(mel::Daq* daq) {
        // make a new Clock and Controller
        mel::Clock clock(1000);
        mel::Controller controller(clock);
        // queue Tasks for the Controller to execute
        mel::Task* task = new TransparencyMode(this, daq);
        controller.queue_task(task);
        // execute the controller
        controller.execute();
        // clean up
        delete task;
    }

    void OpenWrist::TransparencyMode::start() {
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">.";
        getchar();
        daq_->enable();
        std::cout << "Press ENTER to enable OpenWrist.";
        getchar();
        ow_->enable();
        std::cout << "Press ENTER to start the controller.";
        getchar();
        daq_->start_watchdog(0.5);
        std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
    }

    void OpenWrist::TransparencyMode::step() {
        daq_->read_all();
        daq_->reload_watchdog();
        ow_->joints_[0]->set_torque(ow_->compute_gravity_compensation(0) + ow_->compute_friction_compensation(0));
        ow_->joints_[1]->set_torque(ow_->compute_gravity_compensation(1) + ow_->compute_friction_compensation(1));
        ow_->joints_[2]->set_torque(ow_->compute_friction_compensation(2) * 0.5);
        ow_->update_state_map();
        daq_->write_all();
    }

    void OpenWrist::TransparencyMode::stop() {
        ow_->disable();
        daq_->disable();
    }

}