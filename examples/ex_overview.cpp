// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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

#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Mechatronics.hpp>
#include <MEL/Core.hpp>
#include <MEL/Math.hpp>

using namespace mel;

int main(int argc, char* argv[]) {

    Q8Usb daq;                            // instantiate Quanser Q8-USB DAQ
    daq.open();                           // open communication with DAQ
    daq.enable();                         // enable DAQ
    daq.update_input();                   // sync DAQ inputs with real-world
    double voltage = daq.AI.get_value(0); // get voltage of analog input channel 0
    daq.AO[0].set_value(voltage);         // set voltage of analog output channel 0
    daq.update_output();                  // sync DAQ outputs with real-world

    DigitalOutput::Channel do0 = daq.DO[0]; // get digital output channel 0
    AnalogOutput::Channel ao0 = daq.AO[0];  // get analog output channel 0
    Encoder::Channel enc0 = daq.encoder[0]; // get encoder channel 0
    do0.set_value(Low);                     // stage DO0 to be Low TTL
    ao0.set_value(0.0);                     // stage AO0 to be 0.0V
    do0.update();                           // sync DO0
    ao0.update();                           // sync AO0
    enc0.update();                          // sync encoder channel 0
    int32 counts = enc0.get_value();        // get counts on encoder channel 0

    PositionSensor* p = &enc0;
    VelocitySensor* v = static_cast<QuanserEncoder::Channel*>(&enc0);

    Amplifier amp("amc_12a8", High, do0, 1.3, ao0);  // create High enabled PWM amplifier with gain 1.3 A/V
    Motor motor("maxon_re30", 0.0538, amp);          // create DC motor with  torque constant 0.0538 Nm/A
    Joint joint0("axis0", &motor, p, v, 20);         // create a robotic joint with transmission ratio 20
    Robot robot("simple_robot");                     // create a robot
    robot.add_joint(joint0);                         // add joint to robot

    PdController pd(15.0, 0.5);                              // create PD control with gains Kp 15 and Kd 0.5
    Waveform trajectory(Waveform::Sin, seconds(2.0));        // create sinwave trajectory with 2 second period
    double torque, pos_act, vel_act, pos_ref, vel_ref = 0.0; // control variables
    Timer timer(hertz(1000));                                // create 1000 Hz control loop timer
    Time t;                                                  // current time
    while (t < seconds(60)) {                                // loop for 1 minute
        daq.update_input();                                  // sync all DAQ inputs with real-world
        pos_act = robot[0].get_position();                   // get robot joint 0 position
        vel_act = robot[0].get_velocity();                   // get robot joint 0 velocity
        pos_ref = trajectory(t);                             // evaluate trajectory
        torque = pd(pos_act, pos_ref, vel_act, vel_ref);     // calculate PD torque
        robot[0].set_torque(torque);                         // set robot joint 0 torque
        daq.update_output();                                 // sync all DAQ outputs with real-world
        t = timer.wait();                                    // wait for 1 ms to elapse
    }                                                        // end control loop
    daq.disable();                                           // disable DAQ
    daq.close();                                             // close DAQ

    return 0;
}
