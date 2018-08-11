// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#include <MEL/Communications/MelShare.hpp>
#include <MEL/Mechatronics/Motor.hpp>
#include <MEL/Mechatronics/Robot.hpp>
#include <MEL/Mechatronics/VirtualVelocitySensor.hpp>
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Math/Differentiator.hpp>
#include <MEL/Math/Filter.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Mechatronics/PdController.hpp>
#include <MEL/Math/Butterworth.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Math/Integrator.hpp>
#include <fstream>

using namespace mel;

//==============================================================================
// HALL EFFECT SENSOR
//==============================================================================

class HallEffectSensor : public PositionSensor {
public:
    /// Constructor
    HallEffectSensor(const std::string& name,
                     AnalogInput::Channel ai,
                     double gain = 0.0,
                     double offset = 0.0)
        : PositionSensor(name), ai_(ai), gain_(gain), offset_(offset) { }

    /// Gets the position of the hall effect sensor in [rad]
    double get_position() override {
        position_ = ai_.get_value() * gain_ + offset_;
        return position_;
    }

private:

    bool on_enable() override { return true; }
    bool on_disable() override { return true; }

public:
    AnalogInput::Channel ai_;  ///< voltage read from hall effect sensor
    double gain_;              ///< calibration gain   [rad/V]
    double offset_;            ///< calibration offset [rad]
};

//==============================================================================
// HAPTIC PADDLE
//==============================================================================

class HapticPaddle : public Robot {

public:

    /// Constructor
    HapticPaddle(DigitalOutput::Channel d_o,
                 AnalogOutput::Channel ao,
                 AnalogInput::Channel ai) :
          // Robot constructor
          Robot("haptic_paddle"),
          // init amplifier
          amp_("amc_12a8", High, d_o, -1.065, ao),
          // init motor
          motor_("pitman_9434", 0.0229, amp_, Limiter(1.8821, 12.0, seconds(1))),
          // init position sensor
          position_sensor_("honeywell_ss49et", ai),
          // init virtual velocity sensor
          velocity_sensor_("honeywell_ss49et", position_sensor_)
    {
        // create joint
        add_joint(Joint("paddle_joint_0", &motor_, 0.713 / 6.250, &position_sensor_, 1.0,
                        &velocity_sensor_, 1.0, {-50 * DEG2RAD, 50 * DEG2RAD},
                        500 * DEG2RAD, 1.0));
    }

    /// Interactively calibrates the hall effect sensor in the console
    void calibrate() {
        std::ofstream file;
        file.open("calibration.txt");
        std::vector<double> positions = { -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30 };
        print("\nRotate to the indicated position when prompted and hold. Press ENTER to begin calibration.\n", Color::Yellow);
        std::vector<double> volts(positions.size());
        for (std::size_t i = 0; i < positions.size(); ++i) {
            beep();
            std::cout << std::setfill(' ') << std::setw(4) << positions[i] << "  :  ";
            sleep(seconds(3));
            position_sensor_.ai_.update();
            volts[i] = position_sensor_.ai_.get_value();
            std::cout << volts[i] << " V \n";
        }
        std::cout << "\n";
        std::vector<double> mb = linear_regression(volts, positions);
        position_sensor_.gain_ = mb[0];
        position_sensor_.offset_ = mb[1];
        file << mb[0] << "\n";
        file << mb[1] << "\n";
        LOG(Info) << "Calibrated Haptic Paddle hall effect sensor";
        file.close();
    }

private:

    /// Overrides the default Robot::enable function with some custom logic
    bool on_enable() override {
        // load calibration
        std::ifstream file;
        file.open("calibration.txt");
        if (file.is_open()) {
            // read in previous calibration
            double gain, offset;
            file >> gain >> offset;
            position_sensor_.offset_ = offset * DEG2RAD;
            position_sensor_.gain_ = gain * DEG2RAD;
            LOG(Info) << "Imported Haptic Paddle hall effect sensor calibration";
            file.close();
        }
        else {
            file.close();
            calibrate();
        }
        return Robot::enable();
    }

private:
    Amplifier amp_;
    Motor motor_;
    HallEffectSensor position_sensor_;
    VirtualVelocitySensor velocity_sensor_;
};

//==============================================================================
// MISC
//==============================================================================

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC) {
        print("Ctrl+C Pressed!");
        stop = true;
    }
    return true;
}

//==============================================================================
// TUNNEL DEMO
//==============================================================================

Waveform tunnel_trajectory = Waveform(Waveform::Sin, seconds(2), 30.0 * DEG2RAD);
PdController tunnel_pd     = PdController(1.0, 0.01);

double tunnel(double position, double velocity, Time current_time) {
    double x_ref = tunnel_trajectory.evaluate(current_time);
    return tunnel_pd.calculate(x_ref, position, 0, velocity);
}

//==============================================================================
// WALL DEMO
//==============================================================================

double wall_position = 0;
PdController wall_into_pd   = PdController(5.0, 0.1);
PdController wall_outof_pd  = PdController(5.0, 0.0);

double wall(double position, double velocity) {
    if (position > wall_position) {
        if (velocity > 0)
            return wall_into_pd.calculate(wall_position * DEG2RAD, position, 0, velocity);
        else
            return wall_outof_pd.calculate(wall_position * DEG2RAD, position, 0, velocity);
    }
    else
        return 0;
}

//==============================================================================
// NOTCHES DEMO
//==============================================================================

std::vector<double> notch_positions = { -30, -15, 0, 15, 30 };
PdController notch_pd = PdController(3.0, 0.05);

double notches(double position, double velocity) {
    for (std::size_t i = 0; i < notch_positions.size(); ++i) {
        if (position < ((notch_positions[i] + 7.5) * DEG2RAD) &&
            position > ((notch_positions[i] - 7.5) * DEG2RAD)) {
            return notch_pd.calculate(notch_positions[i] * DEG2RAD, position, 0, velocity);
        }
    }
    return 0.0;
}

//==============================================================================
// PENDULUM DEMO
//==============================================================================

class Pendulum {
public:

    Pendulum() : ms("pendulum") {}

    /// Steps the pendulum simulation
    void step_simulation(Time time, double position_ref, double velocity_ref) {

        // compute torque of first joint given reference position and velocity
        Tau[0] = K_player * (position_ref - PI / 2 - Q[0]) + B_player * (velocity_ref - Qd[0]);

        // evaluate the equations of motion
        Qdd[0] = -((L[0] * L[1] * M[1] * mel::sin(Q[1])*pow(Qd[0], 2) - Tau[1] + Fk[1] * mel::tanh(10 * Qd[1]) + B[1] * Qd[1] + g*L[1] * M[1] * mel::cos(Q[0] + Q[1])) / (pow(L[1], 2) * M[1]) - (-L[0] * L[1] * M[1] * mel::sin(Q[1])*pow(Qd[1], 2) - 2 * L[0] * L[1] * M[1] * Qd[0] * mel::sin(Q[1])*Qd[1] - Tau[0] + Fk[0] * mel::tanh(10 * Qd[0]) + B[0] * Qd[0] + g*L[1] * M[1] * mel::cos(Q[0] + Q[1]) + g*L[0] * M[0] * mel::cos(Q[0]) + g*L[0] * M[1] * mel::cos(Q[0])) / (L[1] * M[1] * (L[1] + L[0] * mel::cos(Q[1])))) / ((M[1] * pow(L[1], 2) + L[0] * M[1] * mel::cos(Q[1])*L[1]) / (pow(L[1], 2) * M[1]) - (pow(L[0], 2) * M[0] + pow(L[0], 2) * M[1] + pow(L[1], 2) * M[1] + 2 * L[0] * L[1] * M[1] * mel::cos(Q[1])) / (L[1] * M[1] * (L[1] + L[0] * mel::cos(Q[1]))));
        Qdd[1] = ((-L[0] * L[1] * M[1] * mel::sin(Q[1])*pow(Qd[1], 2) - 2 * L[0] * L[1] * M[1] * Qd[0] * mel::sin(Q[1])*Qd[1] - Tau[0] + Fk[0] * mel::tanh(10 * Qd[0]) + B[0] * Qd[0] + g*L[1] * M[1] * mel::cos(Q[0] + Q[1]) + g*L[0] * M[0] * mel::cos(Q[0]) + g*L[0] * M[1] * mel::cos(Q[0])) / (pow(L[0], 2) * M[0] + pow(L[0], 2) * M[1] + pow(L[1], 2) * M[1] + 2 * L[0] * L[1] * M[1] * mel::cos(Q[1])) - (L[0] * L[1] * M[1] * mel::sin(Q[1])*pow(Qd[0], 2) - Tau[1] + Fk[1] * mel::tanh(10 * Qd[1]) + B[1] * Qd[1] + g*L[1] * M[1] * mel::cos(Q[0] + Q[1])) / (L[1] * M[1] * (L[1] + L[0] * mel::cos(Q[1])))) / (L[1] / (L[1] + L[0] * mel::cos(Q[1])) - (M[1] * pow(L[1], 2) + L[0] * M[1] * mel::cos(Q[1])*L[1]) / (pow(L[0], 2) * M[0] + pow(L[0], 2) * M[1] + mel::pow(L[1], 2) * M[1] + 2 * L[0] * L[1] * M[1] * mel::cos(Q[1])));

        // integrate acclerations to find velocities
        Qd[0] = Qdd2Qd[0].update(Qdd[0], time);
        Qd[1] = Qdd2Qd[1].update(Qdd[1], time);

        // integrate velocities to find positions
        Q[0] = Qd2Q[0].update(Qd[0], time);
        Q[1] = Qd2Q[1].update(Qd[1], time);

        // write out positions
        ms.write_data(Q);
    }

    /// Resets the pendulum integrators
    void reset() {
        Qdd = { 0,0 };
        Qd = { 0,0 };
        Q = { -mel::PI / 2  ,0 };
        Tau = { 0, 0 };
        Qdd2Qd = { mel::Integrator(Qd[0]), mel::Integrator(Qd[1]) };
        Qd2Q = { mel::Integrator(Q[0]), mel::Integrator(Q[1]) };
    }

public:

    double K_player = 1.0;                     ///< [N/m]
    double B_player = 0.2;                    ///< [N-s/m]
    double g = 9.81;                           ///< [m/s^2]
    std::vector<double> M = { 0.01, 0.2 };   ///< [kg]
    std::vector<double> L = { 0.25, 0.25 };    ///< [m]
    std::vector<double> B = { 0.001,0.001 };   ///< [N-s/m]
    std::vector<double> Fk = { 0.001,0.001 };  ///< [Nm]

    std::vector<double> Qdd = { 0,0 };
    std::vector<double> Qd = { 0,0 };
    std::vector<double> Q = { -mel::PI / 2  ,0 };
    std::vector<double> Tau = { 0, 0 };

private:
    MelShare ms;

    std::array<mel::Integrator, 2> Qdd2Qd = { mel::Integrator(Qd[0]), mel::Integrator(Qd[1]) };
    std::array<mel::Integrator, 2> Qd2Q = { mel::Integrator(Q[0]),  mel::Integrator(Q[1]) };
};

Pendulum pend;
int counter = 9;

double pendulum(double position, double velocity, Time current_time) {
    pend.step_simulation(current_time, -position, -velocity);
    return pend.Tau[0];
}


//==============================================================================
// MAIN
//==============================================================================

int main(int argc, char* argv[]) {

    // register CTRL-C handler
    register_ctrl_handler(handler);

    // make and parse console options
    Options options("haptic_paddle.exe", "MEL Haptic Paddle Example");
    options.add_options()
        ("c,calibrate", "Calibrates Hall Effect Sensor")
        ("t,tunnel", "Tunnel demo")
        ("w,wall", "Wall demo")
        ("n,notches", "Notches demo")
        ("p,pendulum", "Pendulum demo")
        ("h,help", "Prints this Help Message");
    auto input = options.parse(argc, argv);

    // print help message if requested
    if (input.count("h") > 0) {
        print(options.help());
        return 0;
    }

    // create MELShare for MELScope
    std::vector<double> to_ms_data(4);
    MelShare to_ms("to_ms");
    std::vector<double> from_ms_data = { 0, 0 };
    MelShare from_ms("from_ms");
    from_ms.write_data(from_ms_data);

    // create Q8 USB
    Q8Usb q8;
    q8.open();

    // create Haptic Paddle
    HapticPaddle hp(q8.DO[7],q8.AO[0],q8.AI[2]);
    // enable Q8 Usb
    q8.enable();

    // enable haptic paddle
    prompt("Press ENTER to enable Haptic Paddle");
    hp.enable();

    // perform calibration if requested
    if (input.count("c") > 0) {
        hp.calibrate();
        return 0;
    }

    // create control loop timer
    Timer timer(hertz(2000));

    // Butterworth filters for position and velocity
    Butterworth buttpos(4, hertz(25), timer.get_frequency());
    Butterworth buttvel(4, hertz(25), timer.get_frequency());
    Butterworth buttorq(2, hertz(10), timer.get_frequency());
    double filtered_pos, filtered_vel;

    // torque
    double torque;

    // enter control loop
    prompt("Press ENTER to start control loop");
    timer.restart();
    while (!stop) {

        // update hardware
        q8.update_input();
        hp[0].get_velocity_sensor<VirtualVelocitySensor>()->update();

        // read from MELScope
        from_ms_data = from_ms.read_data();

        // filter position and velocity
        filtered_pos = buttpos.update(hp[0].get_position());
        filtered_vel = buttvel.update(hp[0].get_velocity());

        // compute torque
        if (input.count("t") > 0)
            torque = tunnel(hp[0].get_position(), filtered_vel, timer.get_elapsed_time_actual());
        else if (input.count("w") > 0)
            torque = wall(hp[0].get_position(), filtered_vel);
        else if (input.count("n") > 0)
            torque = notches(hp[0].get_position(), filtered_vel);
        else if (input.count("p") > 0)
            torque = buttorq.update(pendulum(filtered_pos, filtered_vel, timer.get_elapsed_time()));
        else
            torque = 0;

        // set torque
        hp[0].set_torque(torque);

        // write to MELScope
        to_ms_data[0] = hp[0].get_position() * RAD2DEG;
        to_ms_data[1] = filtered_pos * RAD2DEG;
        to_ms_data[2] = torque;
        to_ms_data[3] = filtered_vel * RAD2DEG;
        to_ms.write_data(to_ms_data);

        // check limits
        if (hp.any_position_limit_exceeded() ||
            hp.any_torque_limit_exceeded() ||
            mel::abs(filtered_vel) > 500 * DEG2RAD)
        {
            LOG(Fatal) << "Haptic Paddle safety limit exceeded";
            stop = true;
        }

        // update hardware output
        q8.update_output();

        // wait timer
        timer.wait();
    }

    return 0;
}
