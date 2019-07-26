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

#include <MEL/Daq/Quanser/QPid.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Devices/Windows/Keyboard.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>

using namespace mel;

// create global stop variable CTRL-C handler function
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

    // create default Q8 USB object
    // (all channels enabled, auto open on, sanity check on)
    QPid qpid;
    if (!qpid.open())
        return 1;


    //==============================================================================
    // ENABLE
    //==============================================================================

    // enable qpid USB
    if (!qpid.enable())
        return 1;

    // qpid.AO[0].set_value(5.0);
    // qpid.update_output();
    // qpid.update_input();
    // double in = qpid.AI[0].get_value();
    // print(in);

    

    qpid.DIO[0].set_direction(Direction::Out);
    qpid.DIO[1].set_direction(Direction::Out);
    qpid.DIO[8].set_direction(Direction::In);
    qpid.DIO[9].set_direction(Direction::In);

    print(qpid.DIO.get_output_channel_numbers());
    print(qpid.DIO.get_input_channel_numbers());

    qpid.DIO[0].set_value(Logic::High);
    qpid.DIO[1].set_value(Logic::Low);

    print(qpid.DIO.get_value(0));
    print(qpid.DIO.get_value(1));

    qpid.update_output();

    prompt("Press ENTER to Measure");

    sleep(seconds(1));

    qpid.update_input();
    double input = qpid.DIO[8].get_value();
    print(input);
    input = qpid.DIO[9].get_value();
    print(input);

    // qpid.watchdog.start();

    // for (int i = 0; i < 1000; ++i) {
    //     qpid.update_input();
    //     print(qpid.encoder[0].get_value());
    //     sleep(milliseconds(1));
    // }

    sleep(seconds(1));

    // qpid.watchdog.stop();
    

    // disable qpid USB
    qpid.disable();
    // close qpid USB
    qpid.close();

    return 0;
}
