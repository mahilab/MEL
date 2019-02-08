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
#include <MEL/Communications.hpp>
#include <MEL/Core.hpp>
#include <MEL/Math.hpp>
#include <MEL/Devices/Windows/Keyboard.hpp>

using namespace mel;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC || event == CtrlEvent::Close) {
        stop = true;
        print("Application Terminated");
    }
    return true;
}

int main() {
    // register handler to gracefully exit program
    register_ctrl_handler(handler);
    // create Q8 USB DAQ
    Q8Usb q8;
    q8.open();
    q8.enable();
    // set encoder deg/count
    q8.encoder[0].set_units_per_count(2 * PI / 500);
    q8.encoder[0].zero();
    // melshare for external scoping
    MelShare ms("pendulum");
    // constants
    const double sense_gain = 2.20; // A/V
    const double commd_gain = 0.60; // A/V
    const double kp = 10.0;         // A/rad
    const double kd = 0.5;          // A*s/rad
    const double ref = PI;          // radians (upright position)
    // PD controlelr (I = 0)
    PidController pd(kp, 0, kd);
    // Configure velocity filter on controller (2nd order, 10 Hz cutoff, Lowpass)
    pd.filter.configure(2, hertz(50), hertz(1000));
    // control loop timer
    Timer timer(hertz(1000));
    // start Q8 watchdog
    q8.watchdog.start();
    while (!stop && !Keyboard::is_key_pressed(Key::Escape)) {
        // get current time
        Time t = timer.get_elapsed_time_actual();
        // sync Q8 input with real world
        q8.update_input();
        // enable/disable pendulum
        if (Keyboard::is_key_pressed(Key::Up))              // enable
            q8.DO[0].set_value(High);
        else if (Keyboard::is_key_pressed(Key::Down))       // disable
            q8.DO[0].set_value(Low);
        // get current pendulum state
        double pos = q8.encoder[0].get_position();          // rad
        // measure actual current
        double sense_volt = q8.AI[0].get_value();           // V
        double sense_curr = sense_volt * sense_gain;        // A
        // compute PD controller command
        double commd_curr = pd.calculate(ref, pos, t);      // A
        // saturate to amplifier max continous
        commd_curr = saturate(commd_curr, 6.0);
        double commd_volt = commd_curr / commd_gain;        // V
        // set command
        q8.AO[0].set_value(commd_volt);
        // write data to melshare for scoping
        ms.write_data({ pos, sense_curr, commd_curr });
        // sync output w/ real world
        q8.update_output();
        // kick watchdog
        q8.watchdog.kick();
        // wait for next timestep
        timer.wait();
    }
    // shutdown Q8
    q8.disable();
    q8.close();
    return 0;
}
