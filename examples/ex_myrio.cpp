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
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Constants.hpp>
#include <string>

using namespace mel;
using namespace std;

// global stop variable 
ctrl_bool stop_flag(false);
// CTRL-C handler function
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC || event == CtrlEvent::Close) {
        stop_flag = true;
        print("Application Terminated");
    }
    return true;
}

/// prints the current myRIO channel configuration
void print_channel_info(const MyRio& myrio) {
    print("----------------------------------------");
    print("Connector MXP A");
    print("    AO: ", myrio.mxpA.AO.get_channel_numbers());
    print("    AI: ", myrio.mxpA.AI.get_channel_numbers());
    print("    DI: ", myrio.mxpA.DIO.get_input_channel_numbers());
    print("    DO: ", myrio.mxpA.DIO.get_output_channel_numbers());
    print("    ENC:", myrio.mxpA.encoder.get_channel_numbers());
    print("Connector MXP B");
    print("    AO: ", myrio.mxpB.AO.get_channel_numbers());
    print("    AI: ", myrio.mxpB.AI.get_channel_numbers());
    print("    DI: ", myrio.mxpB.DIO.get_input_channel_numbers());
    print("    DO: ", myrio.mxpB.DIO.get_output_channel_numbers());
    print("    ENC:", myrio.mxpB.encoder.get_channel_numbers());
    print("Connector MSP C");
    print("    AO: ", myrio.mspC.AO.get_channel_numbers());
    print("    AI: ", myrio.mspC.AI.get_channel_numbers());
    print("    DI: ", myrio.mspC.DIO.get_input_channel_numbers());
    print("    DO: ", myrio.mspC.DIO.get_output_channel_numbers());  
    print("    ENC:", myrio.mspC.encoder.get_channel_numbers());
    print("----------------------------------------");
}

// main
int main(int argc, char** argv) {

    // register our handler so that the program gracefully exits
    // when CTRL-C is pressed in the terminal
    register_ctrl_handler(handler);

    // set up user options to pass in through command line
    Options options("myrio", "myRIO Example");
    options.add_options()
        ("l,local",  "Local Port for MelNet",        value<unsigned short>())
        ("r,remote", "Remote Port for MelNet",       value<unsigned short>())
        ("i,ip",     "Remote IP Address for MelNet", value<string>())
        ("x,reset",  "Resets myRIO on open")
        ("h,help",   "Help");
    // parse options user passes in
    auto user = options.parse(argc, argv);
    // print help info if user requests it
    if (user.count("help") > 0) {
        print(options.help());
        return 0;
    }
    
    // create MyRio object
    MyRio myrio;
    if (!myrio.open())
        return 1;

    if (user.count("x"))
        myrio.reset();

    myrio.mspC.encoder.enable_channel(0);

    myrio.mspC.DIO[1].set_direction(Out);
    myrio.mspC.DIO[5].set_direction(In);

    print_channel_info(myrio);

    // set units per count on encoder
    myrio.mspC.encoder[0].set_units_per_count(2 * PI / 500.0f);

    // enable myrio
    myrio.enable();

    // create MelNet to stream data to a remote PC
    unsigned short local_port  = user.count("l") ? user["l"].as<unsigned short>() : 55002;
    unsigned short remote_port = user.count("r") ? user["r"].as<unsigned short>() : 55001;
    string remote_ip_address   = user.count("i") ? user["ip"].as<string>() : "172.22.11.1";
    string local_ip_address    = IpAddress::get_local_address().to_string();

    MelNet mn(local_port, remote_port, remote_ip_address);

    print("MelNet Settings:");
    print("Local Port: ", local_port);
    print("Local IP:   ", local_ip_address);
    print("Remote Port:", remote_port);
    print("Remote IP:  ", remote_ip_address);

    // create sinwave for analog loopback
    Waveform sinwave(Waveform::Sin, seconds(1), 10);

    // create 1000Hz loop Timer and Time t
    Timer timer(hertz(1000), Timer::Sleep);
    Time t;

    // control loop
    while (!stop_flag) {
        // update myrio inputs
        myrio.update_input();
        // perform analog loopback
        double voltage_write = sinwave(t);
        myrio.mspC.AO[0].set_value(voltage_write);
        double voltage_read  = myrio.mspC.AI[0].get_value(); 
        // button -> DO[1] -> DI[5] -> led[3] loop
        if (myrio.is_button_pressed())
            myrio.mspC.DIO[1].set_value(High);
        else
            myrio.mspC.DIO[1].set_value(Low);
        if (myrio.mspC.DIO[5] == High)
            myrio.set_led(3, true);
        else
            myrio.set_led(3, false);
        // read an encoder
        int    counts   = myrio.mspC.encoder[0].get_value();
        double position = myrio.mspC.encoder[0].get_position(); // counts scale by 2*PI/500.0f
        if (myrio.is_button_pressed()) 
            myrio.mspC.encoder[0].zero();
        // send myRIO voltages and encoder position over MelNet
        mn.send_data({voltage_write, voltage_read, (double)counts, position});
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
