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

#include <MEL/Communications/MelNet.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Core/Clock.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <MEL/Math/Chirp.hpp>

// To see this example in action, start this exectable then run MelScope.pyw
// and open the provided example.scope file.

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main(int argc, char* argv[]) {
    Options options("melscope.exe", "MELScope Demo");
    options.add_options()("r", "Remote Address", value<std::string>());
    auto result              = options.parse(argc, argv);
    IpAddress remote_address = "127.0.0.1";
    if (result.count("r") > 0)
        remote_address = IpAddress(result["r"].as<std::string>());

    // register CTRL-C handler
    register_ctrl_handler(handler);

    // make MelNet
    MelNet mn(55001, 55002, remote_address, false);
    print("MELNet: (lp:55002, rp:55001, ra:" + remote_address.to_string() +
          ")");

    // make MelShare (Windows only, and prefered over MelNet)
    MelShare ms("melscope");
    print("MELShare: \"melscope\"");

    // create data buffers so we don't have to make them in each loop iteration
    std::vector<double> data_ms(4);
    std::vector<double> data_mn(2);

    // create waveforms
    Waveform sin_wave(Waveform::Sin, seconds(2));
    Waveform sqr_wave(Waveform::Square, seconds(2));
    Waveform tri_wave(Waveform::Triangle, seconds(2));
    Waveform saw_wave(Waveform::Sawtooth, seconds(2));
    Chirp    chirp1(hertz(0.1), hertz(1), seconds(60));
    Chirp    chirp2(hertz(0.1), hertz(1), seconds(60));

    print("Running MEL Scope Example ... ");

    Timer timer(milliseconds(1), Timer::Hybrid);
    while (!stop) {
        // evaluate waveforms
        Time t = timer.get_elapsed_time();
        data_ms[0] = sin_wave(t);
        data_ms[1] = sqr_wave(t);
        data_ms[2] = chirp1(t);
        data_ms[3] = chirp2(t);

        data_mn[0] = tri_wave(t);
        data_mn[1] = saw_wave(t);

        // update MelNet
        if (mn.check_request())
            mn.send_data(data_mn);

#ifdef _WIN32
        // update MelShare
        ms.write_data(data_ms);
#endif

        timer.wait();
    }

    print("Terminated");

    return 0;
}
