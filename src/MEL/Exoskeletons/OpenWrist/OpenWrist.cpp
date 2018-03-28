#include <MEL/Exoskeletons/OpenWrist/OpenWrist.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Core/PositionSensor.hpp>
#include <MEL/Core/VelocitySensor.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

OpenWrist::OpenWrist(OwConfiguration configuration, OwParameters parameters) :
    Exo("open_wrist"),
    config_(configuration),
    params_(parameters)
{
    // reserve space for motors so they don't get reallocated before passing to Joint()
    motors_.reserve(3);

    // construct each motor and joint
    for (int i = 0; i < 3; ++i) {

        std::string num = stringify(i);

        // construct motors
        motors_.push_back(Motor("ow_motor_" + num,
            params_.kt_[i],
            config_.amplifiers_[i],
                Limiter(params_.motor_cont_limits_[i],
                params_.motor_peak_limits_[i],
                params_.motor_i2t_times_[i])));

        // set encoder counts
        config_.encoder_channels_[i].set_units_per_count(2 * PI / params_.encoder_res_[i]);
        config_.velocity_channels_[i].set_units_per_count(2 * PI / params_.encoder_res_[i]);

        // create joint
        Joint joint(
            "ow_joint_" + num,
            motors_[i],
            params_.eta_[i],
            config_.encoder_channels_[i],
            params_.eta_[i],
            config_.velocity_channels_[i],
            params_.eta_[i],
            std::array<double, 2>({ params_.pos_limits_neg_[i] , params_.pos_limits_pos_[i] }),
            params_.vel_limits_[i],
            params_.joint_torque_limits[i]);

        add_joint(joint);
    }
}

OpenWrist::~OpenWrist() {
    if (is_enabled())
        disable();
}

double OpenWrist::compute_gravity_compensation(uint32 joint) {
    std::vector<double> q = get_joint_positions();
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
    return params_.kin_friction_[joint] * tanh(10.0 * joints_[joint].get_velocity());
}

std::array<double, 3> OpenWrist::compute_friction_compensation() {
    std::array<double, 3> friction_torques = { 0,0,0 };
    for (int joint = 0; joint < 3; joint++) {
        friction_torques[joint] = params_.kin_friction_[joint] * tanh(10.0 * joints_[joint].get_velocity());
    }
    return friction_torques;
}

std::array<double, 3> OpenWrist::compute_gravity_compensation() {
    std::vector<double> q = get_joint_positions();
    std::array<double, 3> gravity_torques = { 0,0,0 };
    gravity_torques[0] = 1.3939*sin(q[0]) - 0.395038*cos(q[0]) + 0.351716*cos(q[0])*cos(q[1]) + 0.0826463*cos(q[0])*sin(q[1]) + 0.0397738*cos(q[2])*sin(q[0]) - 0.0929844*sin(q[0])*sin(q[2]) - 0.0929844*cos(q[0])*cos(q[2])*sin(q[1]) - 0.0397738*cos(q[0])*sin(q[1])*sin(q[2]);
    gravity_torques[1] = -8.50591e-18*sin(q[0])*(4.13496e16*sin(q[1]) - 9.71634e15*cos(q[1]) + 1.09317e16*cos(q[1])*cos(q[2]) + 4.67601e15*cos(q[1])*sin(q[2]));
    gravity_torques[2] = 0.0929844*cos(q[0])*cos(q[2]) + 0.0397738*cos(q[0])*sin(q[2]) + 0.0929844*sin(q[0])*sin(q[1])*sin(q[2]) - 0.0397738*cos(q[2])*sin(q[0])*sin(q[1]);
    return gravity_torques;
}


void OpenWrist::calibrate(volatile std::atomic<bool>& stop) {

    // create needed variables
    std::array<double, 3> zeros = { 0, 0, 0 }; // determined zero positions for each joint
    std::array<int, 3> dir = { -1 , 1, -1 };   // direction to rotate each joint
    mel::uint32 calibrating_joint = 0;         // joint currently calibrating
    bool returning = false;                    // bool to track if calibrating joint is return to zero
    double pos_ref = 0;                        // desired position
    double vel_ref = 90 * DEG2RAD;             // desired velocity
    std::vector<double> stored_positions;      // stores past positions
    stored_positions.reserve(100000);

    std::array<double, 3> sat_torques = { 2.0, 0.5, 1.0 }; // temporary saturation torques

    Time timeout = seconds(10); // max amout of time we will allow calibration to occur for

    // enable DAQs, zero encoders, and start watchdog
    config_.daq_.enable();
    config_.encoder_channels_[0].zero();
    config_.encoder_channels_[1].zero();
    config_.encoder_channels_[2].zero();
    config_.watchdog_.start();

    // enable OpenWrist
    enable();

    // start the clock
    Timer timer(milliseconds(1), Timer::Hybrid);

    // start the calibration control loop
    while (!stop && timer.get_elapsed_time() < timeout) {

        // read and reload DAQs
        config_.daq_.update_input();
        config_.watchdog_.kick();

        // iterate over all joints
        for (std::size_t i = 0; i < 3; i++) {

            // get positions and velocities
            double pos_act = joints_[i].get_position();
            double vel_act = joints_[i].get_velocity();

            double torque = 0;
            if (i == calibrating_joint) {

                if (!returning) {

                    // calculate torque req'd to move the calibrating joint forward at constant speed
                    pos_ref += dir[i] * vel_ref * timer.get_period().as_seconds();
                    torque = pd_controllers_[i].calculate(pos_ref, pos_act, 0, vel_act);
                    torque = saturate(torque, sat_torques[i]);

                    // check if the calibrating joint is still moving
                    stored_positions.push_back(pos_act);
                    bool moving = true;
                    if (stored_positions.size() > 500) {
                        moving = false;
                        for (size_t j = stored_positions.size() - 500; j < stored_positions.size(); j++) {
                            moving = stored_positions[j] != stored_positions[j - 1];
                            if (moving)
                                break;
                        }
                    }

                    // if it's not moving, it's at a hardstop so record the position and deduce the zero location
                    if (!moving) {
                        if (dir[i] > 0)
                            zeros[i] = pos_act - params_.pos_limits_pos_[i];
                        else if (dir[i] < 0)
                            zeros[i] = pos_act - params_.pos_limits_neg_[i];
                        returning = true;
                    }
                }

                else {
                    // calculate torque req'd to retur the calibrating joint back to zero
                    pos_ref -= dir[i] * vel_ref *  timer.get_period().as_seconds();
                    torque = pd_controllers_[i].calculate(pos_ref, pos_act, 0, vel_act);
                    torque = saturate(torque, sat_torques[i]);


                    if (dir[i] * pos_ref <= dir[i] * zeros[i]) {
                        // reset for the next joint
                        calibrating_joint += 1;
                        pos_ref = 0;
                        returning = false;
                        LOG(Info) << "Joint " << joints_[i].get_name() << " calibrated";
                    }
                }

            }
            else {
                // lock all other joints at their zero positions
                torque = pd_controllers_[i].calculate(zeros[i], pos_act, 0, vel_act);
                torque = saturate(torque, sat_torques[i]);

            }
            joints_[i].set_torque(torque);
        }

        // write all DAQs
        config_.daq_.update_output();

        // check joint velocity limits
        if (any_velocity_limit_exceeded() && any_torque_limit_exceeded()) {
            stop = true;
            break;
        }

        // wait the clock
        timer.wait();
    }

    // disable OpenWrist
    disable();

    // zero DAQs and disable them
    config_.encoder_channels_[0].zero();
    config_.encoder_channels_[1].zero();
    config_.encoder_channels_[2].zero();
    config_.daq_.disable();
}


void OpenWrist::transparency_mode(volatile std::atomic<bool>& stop, bool friction_compensation) {

    // enable DAQs, zero encoders, and start watchdog
    config_.daq_.enable();
    config_.watchdog_.start();

    // enable OpenWrist
    enable();

    // create and start the clock
    Timer timer(milliseconds(1), Timer::Hybrid);

    while (!stop) {

        // read and reload DAQs
        config_.daq_.update_input();
        config_.watchdog_.kick();

        // calculate and set compensation torques
        joints_[0].set_torque(compute_gravity_compensation(0)); 
        joints_[1].set_torque(compute_gravity_compensation(1));
        joints_[2].set_torque(compute_gravity_compensation(2));

        if (friction_compensation) {
            joints_[0].add_torque(compute_friction_compensation(0));
            joints_[1].add_torque(compute_friction_compensation(1));
            joints_[2].add_torque(compute_friction_compensation(2));
        }


        // write all DAQs
        config_.daq_.update_output();

        // check joint limits
        if (any_torque_limit_exceeded() && any_velocity_limit_exceeded()) {
            stop = true;
            break;
        }

        // wait the clock
        timer.wait();
    }

    // disable OpenWrist
    disable();

    // disable all DAQs
    config_.daq_.disable();
}

} // namespace mel
