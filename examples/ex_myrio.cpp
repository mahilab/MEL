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

#include <MEL/Core.hpp>
#include <MEL/Utility/Options.hpp>
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

    // user options
    Options options("myrio", "myRIO Example");
    options.add_options()
        ("l,local",    "Local Port for MelNet",        value<unsigned short>())
        ("r,remote",   "Remote Port for MelNet",       value<unsigned short>())
        ("i,ip",       "Remote IP Address for MelNet", value<string>())
        ("h,help",     "Help");
    auto user = options.parse(argc, argv);
    if (user.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // register handler to gracefully exit program
    register_ctrl_handler(handler);

    // see if user passed us custom ports/ip address for MelNet
    unsigned short local_port  = user.count("l") ? user["l"].as<unsigned short>() : 55002;
    unsigned short remote_port = user.count("r") ? user["r"].as<unsigned short>() : 55001;
    string remote_ip_address   = user.count("i") ? user["ip"].as<string>() : "172.22.11.1";

    // create MelNet
    print("Local Port:", local_port);
    print("Remote Port:", remote_port);
    print("Remote IP:", remote_ip_address);

    MelNet mn(local_port, remote_port, remote_ip_address);

    // create MyRio object
    MyRio myrio;
    if (!myrio.open())
        return 1;

    // myrio.mxpA.configure_encoders(1);
    // myrio.mspC.configure_encoders(2);
    // myrio.mxpB.DIO.set_direction(5, Out);
    // myrio.mxpB.DIO.set_direction(6, Out);

    // Print channel information
    print("Connector MXP A");
    print("  AO: ",myrio.mxpA.AO.get_channel_numbers());
    print("  AI: ",myrio.mxpA.AI.get_channel_numbers());
    print("  DI: ", myrio.mxpA.DIO.get_input_channel_numbers());
    print("  DO: ", myrio.mxpA.DIO.get_output_channel_numbers());
    print("  ENC:",myrio.mxpA.encoder.get_channel_numbers());
    print("Connector MXP B");
    print("  AO: ",myrio.mxpB.AO.get_channel_numbers());
    print("  AI: ",myrio.mxpB.AI.get_channel_numbers());
    print("  DI: ", myrio.mxpB.DIO.get_input_channel_numbers());
    print("  DO: ", myrio.mxpB.DIO.get_output_channel_numbers());
    print("  ENC:",myrio.mxpB.encoder.get_channel_numbers());
    print("Connector MSP C");
    print("  AO: ",myrio.mspC.AO.get_channel_numbers());
    print("  AI: ",myrio.mspC.AI.get_channel_numbers());
    print("  DI: ", myrio.mspC.DIO.get_input_channel_numbers());
    print("  DO: ", myrio.mspC.DIO.get_output_channel_numbers());
    print("  ENC:",myrio.mspC.encoder.get_channel_numbers());

    myrio.enable();

    // set units per count on encoder
    myrio.mspC.encoder[0].set_units_per_count(2*PI/500.0f);

    // create sinwave for loopback
    Waveform sinwave(Waveform::Sin, seconds(1), 10);

    // create loop Timer and Time t
    Timer timer(hertz(1000), Timer::Sleep);
    Time t;

    // loop
    while (!stop) {
        // update myrio inputs
        myrio.update_input();
        // perform analog loopback
        double voltage_write = sinwave(t);
        myrio.mspC.AO[0].set_value(voltage_write);
        double voltage_read  = myrio.mspC.AI[0].get_value(); 
        // read an encoder
        double position = myrio.mspC.encoder[0].get_position();
        // send myRIO voltages and encoder position over MelNet
        mn.send_data({voltage_write, voltage_read, position});
        // update myrio outputs
        myrio.update_output();
        // wait timer
        t = timer.wait();
    }

    // shutdown myrio
    myrio.disable();
    myrio.close();
    return 0;
}
