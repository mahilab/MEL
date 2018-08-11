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

#include <MEL/Daq/Quanser/Q2Usb.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Devices/Windows/Keyboard.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main() {

    // register CTRL-C handler
    register_ctrl_handler(handler);

    //==============================================================================
    // CONSTUCT/OPEN/CONFIGURE
    //==============================================================================

    // create default Q28 USB object (all channels enabled)
    Q2Usb q2;
    q2.open();

    //==============================================================================
    // ENABLE
    //==============================================================================

    // ask for user input
    prompt("Press ENTER to open and enable Q8 USB.");
    // enable Q8 USB
    q2.enable();

    //==============================================================================
    // ENCODER
    //==============================================================================

    // ask for user input
    prompt("Connect an encoder to channel 0 then press ENTER to start test.");
    // create 10 Hz Timer
    Timer timer(milliseconds(100));
    // start encoder loop
    while (timer.get_elapsed_time_actual() < seconds(5) && !stop) {
        q2.update_input();
        print(q2.encoder.get_value(0));
        timer.wait();
    }

    //==============================================================================
    // ANALOG INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect AO0 to AI0 then press ENTER to start test.");
    // create Waveform
    Waveform wave(Waveform::Sin, seconds(0.25), 5.0);
    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time_actual() < seconds(5) && !stop) {
        q2.update_input();
        print(q2.AI.get_value(0));
        double voltage = wave.evaluate(timer.get_elapsed_time_actual());
        q2.AO.set_value(0, voltage);
        q2.update_output();
        timer.wait();
    }

    //==============================================================================
    // DIGITAL INPUT/OUTPUT
    //==============================================================================

    // ask for user input
    prompt("Connect DIO0 to DIO7 then press ENTER to start test.");
    // set directions
    q2.DIO.set_direction(7, Out);
    Logic signal = High;
    // start analog loopback loop
    timer.restart();
    while (timer.get_elapsed_time_actual() < seconds(5) && !stop) {
        q2.DIO.update();
        print((int)q2.DIO[0].get_value());
        signal = (Logic)(High - signal);
        q2.DIO[7].set_value(signal);
        q2.DIO.update();
        timer.wait();
    }

    //==============================================================================
    // LED
    //==============================================================================

    // ask for user input
    prompt("Press enter then hold L to turn on LED.");
    // make sure LED Mode is set to User
    QuanserOptions options;
    options.set_led_mode(QuanserOptions::LedMode::User);
    q2.set_options(options);
    timer.restart();
    while (timer.get_elapsed_time_actual() < seconds(10) && !stop) {
        if (Keyboard::is_key_pressed(Key::L))
            q2.set_led(High);
        else
            q2.set_led(Low);
        q2.update_output();
        timer.wait();
    }

    //==============================================================================
    // DISABLE
    //==============================================================================

    // ask for user input
    prompt("Press Enter to disable the Q2 USB.");
    // disable Q8 USB
    q2.disable();
    // close Q8 USB
    q2.close();

    return 0;
}
