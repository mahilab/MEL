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

#include <MEL/Utility/Options.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <MEL/Communications/MelNet.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Math.hpp>

/* This demo demonstates how MelNet and MelShare can by combined to create a
 * communications server. The server constantly checks for incomming data from a
 * remote client over MelNet, and writes received data to a MelShare. Ideally,
 * this server would be run in the background on a host PC. Thus, the recieved
 * data becomes globally available on the host PC simply by accessing the
 * MelShare, e.g. over MelScope or through another C++, C#, or Python
 * application using the MelShare classes.
 *
 * The server can be demoed by executing the following commands:
 *
 *  Server > comms_server.exe -l PORT_A -r PORT_B -i DEMO_IP -m MELSHARE
 *  Demo   > comms_server.exe -l PORT_B -r PORT_A -i SERVER_IP -d
 *
 *  where PORT_A and PORT_B are valid port numbers (e.g. 55001 and 55002),
 *  DEMO_IP and SERVER_IP are the IP addresses of the server and demo, and
 *  MELSHARE is the desired MelShare string name. Open the MelShare in MelScope.
 *
 *  Note: The demo can be run on a single PC by using two consoles and
 *  leaving the -i option unspecied (it will default to the local host IP).
 */

using namespace mel;

int main(int argc, char* argv[]) {

    // setup and parse console input options
    Options options("comms_server.exe", "MEL Communications Server");
    options.add_options()
        ("l,local",    "Local Port",           value<unsigned short>())
        ("r,remote",   "Remote Port",          value<unsigned short>())
        ("i,ip",       "Remote IP Address",    value<std::string>())
        ("m,melshare", "MelShare String Name", value<std::string>())
        ("d,demo",     "Simulates Fake Client")
        ("h,help",     "Help");
    auto user_input = options.parse(argc, argv);
    if (user_input.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // default settings
    unsigned short local  = 55001;
    unsigned short remote = 55002;
    IpAddress ip = "127.0.0.1";
    std::string melshare_name = "comms_server";

    // set user settings if provided
    if (user_input.count("local"))
        local = user_input["local"].as<unsigned short>();
    if (user_input.count("remote"))
        remote = user_input["remote"].as<unsigned short>();
    if (user_input.count("ip"))
        ip = user_input["ip"].as<std::string>();
    if (user_input.count("melshare"))
        melshare_name = user_input["melshare"].as<std::string>();

    // log settings
    LOG(Info) << "Local IP:    " << IpAddress::get_local_address();
    LOG(Info) << "Local Port:  " << local;
    LOG(Info) << "Remote IP:   " << ip;
    LOG(Info) << "Remote Port: " << remote;
    LOG(Info) << "MelShare:    " << melshare_name;

    // create MelNet
    MelNet melnet(local,remote,ip);

    // demo loop
    if (user_input.count("demo")) {
        LOG(Info) << "Starting Demo";
        Waveform sinwave(Waveform::Sin, seconds(1));
        Waveform triwave(Waveform::Triangle, seconds(1));
        Timer timer(hertz(1000));
        while (true) {
            Time t = timer.get_elapsed_time();
            melnet.send_data({sinwave(t),triwave(t)});
            timer.wait();
        }
    }
    // server loop
    else {
        LOG(Info) << "Starting Server";
        MelShare melshare(melshare_name);
        std::vector<double> data;
        while (true) {
            data = melnet.receive_data();
            melshare.write_data(data);
        }
    }

    return 0;
}
