#include "OpenWrist.h"
#include "Encoder.h"
#include "Motor.h"
#include "Input.h"

namespace mel {

    namespace exo {

        OpenWrist::OpenWrist(Config configuration, Params parameters) :
            Exo("open_wrist"),
            config_(configuration),
            params_(parameters)
        {
            // for every joint
            for (int i = 0; i < 3; i++) {
                std::string num = std::to_string(i);

                // construct encoders
                core::PositionSensor* encoder = new core::Encoder("encoder_" + num,
                    params_.encoder_res_[i] / (2 * math::PI),
                    config_.encoder_[i],
                    config_.encrate_[i]);

                position_sensors_.push_back(encoder);

                // construct motors
                core::Actuator* motor = new core::Motor("motor_" + num,
                    params_.kt_[i],
                    config_.amp_gains_[i],
                    config_.command_[i],
                    config_.enable_[i],
                    core::Actuator::EnableMode::High,
                    config_.sense_[i],
                    params_.motor_cont_limits_[i],
                    params_.motor_peak_limits_[i],
                    params_.motor_i2t_times_[i]);

                actuators_.push_back(motor);

                // construct joints
                core::Joint* joint = new core::Joint("joint_" + num,
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

            state_[9] = static_cast<core::Motor*>(joints_[0]->actuator_)->get_current_command();
            state_[10] = static_cast<core::Motor*>(joints_[1]->actuator_)->get_current_command();
            state_[11] = static_cast<core::Motor*>(joints_[2]->actuator_)->get_current_command();

            state_[12] = static_cast<core::Motor*>(joints_[0]->actuator_)->get_current_sense();
            state_[13] = static_cast<core::Motor*>(joints_[1]->actuator_)->get_current_sense();
            state_[14] = static_cast<core::Motor*>(joints_[2]->actuator_)->get_current_sense();

            state_map_.write(state_);
        }

        void OpenWrist::add_daq(core::Daq* daq) {
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


        void OpenWrist::calibrate() {

            // create needed variables 
            std::array<double, 3> zeros = { 0, 0, 0 }; // determined zero positions for each joint
            std::array<int, 3> dir = { -1 , 1, -1 };   // direction to rotate each joint
            mel::uint32 calibrating_joint = 0;         // joint currently calibrating
            bool returning = false;                    // bool to track if calibrating joint is return to zero
            double pos_ref = 0;                        // desired position
            double vel_ref = 90 * math::DEG2RAD;             // desired velocity
            std::vector<double> stored_positions;      // stores past positions
            stored_positions.reserve(100000);
            util::Clock clock(1000);

            std::array<double, 3> sat_torques = { 2.0, 0.5, 1.0 }; // temporary saturation torques

            double timeout = 20; // max amout of time we will allow calibration to occur for
            bool stop = false;   // flag used to exit calibration loop

            // enable DAQs, zero encoders, and start watchdog
            for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                (*it)->enable();
                (*it)->zero_encoders();
                (*it)->start_watchdog(0.1);
            }

            // enable OpenWrist
            enable();

            // start the clock
            clock.start();

            // start the calibration control loop
            while (clock.time() < timeout && !stop) {

                // read and reload DAQs
                for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                    (*it)->read_all();
                    (*it)->reload_watchdog();
                }

                // iterate over all joints
                for (int i = 0; i < 3; i++) {

                    // get positions and velocities
                    double pos_act = joints_[i]->get_position();
                    double vel_act = joints_[i]->get_velocity();

                    double torque = 0;
                    if (i == calibrating_joint) {

                        if (!returning) {

                            // calculate torque req'd to move the calibrating joint forward at constant speed
                            pos_ref += dir[i] * vel_ref * clock.delta_time_;
                            torque = pd_controllers[i].calculate(pos_ref, pos_act, 0, vel_act);
                            torque = math::saturate(torque, sat_torques[i]);


                            // check if the calibrating joint is still moving
                            stored_positions.push_back(pos_act);
                            bool moving = true;
                            if (stored_positions.size() > 500) {
                                moving = false;
                                for (size_t i = stored_positions.size() - 500; i < stored_positions.size(); i++) {
                                    moving = stored_positions[i] != stored_positions[i - 1];
                                    if (moving)
                                        break;
                                }
                            }

                            // if it's not moving, it's at a hardstop so record the position and deduce the zero location
                            if (!moving) {
                                std::cout << "Joint <" << joints_[i]->name_ << "> reached the reference position. Returning to zero ... ";
                                if (dir[i] > 0)
                                    zeros[i] = pos_act - params_.pos_limits_pos_[i];
                                else if (dir[i] < 0)
                                    zeros[i] = pos_act - params_.pos_limits_neg_[i];
                                returning = true;
                            }
                        }

                        else {
                            // calculate torque req'd to retur the calibrating joint back to zero
                            pos_ref -= dir[i] * vel_ref * clock.delta_time_;
                            torque = pd_controllers[i].calculate(pos_ref, pos_act, 0, vel_act);
                            torque = math::saturate(torque, sat_torques[i]);


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
                        torque = pd_controllers[i].calculate(zeros[i], pos_act, 0, vel_act);
                        torque = math::saturate(torque, sat_torques[i]);

                    }
                    joints_[i]->set_torque(torque);
                }

                // update state map
                update_state_map();

                // check for stop input from user
                stop = (util::Input::is_key_pressed(util::Input::LControl) && util::Input::is_key_pressed(util::Input::C));

                // write all DAQs
                for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                    (*it)->write_all();
                }

                // check joint velocity limits
                if (check_all_joint_velocity_limits()) {
                    stop = true;
                    break;
                }

                // wait the clock
                clock.hybrid_wait();
            }

            // disable OpenWrist
            disable();

            // zero DAQs and disable them
            for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                (*it)->zero_encoders();
                (*it)->disable();
            }

        }

        void OpenWrist::transparency_mode() {

            util::Clock clock(1000);
            bool stop = false;

            // enable DAQs, zero encoders, and start watchdog
            for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                (*it)->enable();
                (*it)->start_watchdog(0.1);
            }

            // enable OpenWrist
            enable();

            // create and start the clock
            clock.start();

            while (!stop) {

                // read and reload DAQs
                for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                    (*it)->read_all();
                    (*it)->reload_watchdog();
                }

                // calculate and set compensation torques
                joints_[0]->set_torque(compute_gravity_compensation(0) + compute_friction_compensation(0));
                joints_[1]->set_torque(compute_gravity_compensation(1) + compute_friction_compensation(1));
                joints_[2]->set_torque(compute_friction_compensation(2) * 0.5);

                // update state map
                update_state_map();

                // check for stop input from user
                stop = (util::Input::is_key_pressed(util::Input::LControl) && util::Input::is_key_pressed(util::Input::C));

                // write all DAQs
                for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                    (*it)->write_all();
                }

                // check joint limits
                if (check_all_joint_torque_limits() && check_all_joint_velocity_limits()) {
                    stop = true;
                    break;
                }

                // wait the clock
                clock.hybrid_wait();
            }

            // disable OpenWrist
            disable();

            // disable all DAQs
            for (auto it = daqs_.begin(); it != daqs_.end(); ++it) {
                (*it)->disable();
            }

        }

    }

}