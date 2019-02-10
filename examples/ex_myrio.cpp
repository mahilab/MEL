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

#include <MEL/Core/Timer.hpp>
#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Communications/MelNet.hpp>
#include <string>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Constants.hpp>

using namespace mel;
using namespace std;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC || event == CtrlEvent::Close) {
        stop = true;
        print("Application Terminated");
    }
    return true;
}

// main
int main(int argc, char** argv) {

    // register handler to gracefully exit program
    register_ctrl_handler(handler);

    // MelNet to send data to host PC (port 55001@172.22.11.1 from local port 55002)
    MelNet mn(55002, 55001, "172.22.11.1");

    // create MyRio object
    MyRio myrio("myrio");
    if (!myrio.open() || !myrio.enable())
        return 1;

    // set units per count on encoder
    myrio.C.encoder[0].set_units_per_count(2*PI/500.0f);

    // create sinwave for loopback
    Waveform sinwave(Waveform::Sin, seconds(1), 10);

    // create loop Timer and Time t
    Timer timer(hertz(1000));
    Time t;

    // loop
    while (!stop) {
        t = timer.get_elapsed_time_actual();
        // update myrio inputs
        myrio.update_input();
        // perform analog loopback
        double voltage_write = sinwave(t);
        myrio.C.AO[0].set_value(voltage_write);
        double voltage_read  = myrio.C.AI[0].get_value();
        // read an encoder
        double position = myrio.C.encoder[0].get_position();
        // send myRIO encoder position over MelNet
        mn.send_data({voltage_write, voltage_read, position});
        // update myrio outputs
        myrio.update_output();
        // wait timer
        timer.wait();
    }

    // shutdown myrio
    myrio.disable();
    myrio.close();
    return 0;
}
