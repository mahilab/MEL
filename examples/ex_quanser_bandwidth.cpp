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

#include <MEL/Daq/Quanser/Q2Usb.hpp>
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Daq/Quanser/QPid.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Devices/Windows/Keyboard.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Csv.hpp>


using namespace mel;

template <typename TDaq>
int run_test(TDaq& daq, int frequency, int time, int input_ch, int output_ch) {
    if (!(daq.open() && daq.enable())) {
        LOG(Error) << "[Bandwidth] Failed to open and enable DAQ";
        return 1;
    }   
    int iterations = frequency * time;
    LOG(Info) << "[Bandwidth] Starting Test";
    LOG(Info) << "[Bandwidth] Frequency:        " << frequency << " Hz";
    LOG(Info) << "[Bandwidth] Duration:         " << time << " s";
    LOG(Info) << "[Bandwidth] Iterations:       " << iterations;
    LOG(Info) << "[Bandwidth] I/O Channels:     " << input_ch << "/" << output_ch;
    std::vector<std::array<double,2>> data(iterations);
    Timer timer(hertz(frequency));
    timer.set_acceptable_miss_rate(2); // prevent warnings
    Time t = Time::Zero;
    Waveform sinwave(Waveform::Sin, hertz(100), 10);

    // daq.AI.set_channel_numbers({0,1,2,3});
    // daq.AO.set_channel_numbers({0});

    Clock clock;
    for (int i = 0; i < iterations; ++i) {
        daq.AI.update();
        data[i][0] = clock.get_elapsed_time().as_seconds();
        data[i][1] = daq.AI[input_ch];
        daq.AO[output_ch] = sinwave(clock.get_elapsed_time());
        daq.AO.update();
        // wait
        timer.wait();
    }

    Time testDur = clock.get_elapsed_time();

    // sum repeated measures
    int repeats = 0;
    for (int i = 1; i < iterations; ++i) {
        if (data[i][1] == data[i-1][1])
            repeats++;
    }

    LOG(Info) << "[Bandwidth] Test Completed";
    LOG(Info) << "[Bandwidth] Repeats:          " << repeats;
    LOG(Info) << "[Bandwidth] Repeat Rate:      " << (double)repeats / (double)(iterations - 1);
    LOG(Info) << "[Bandwidth] Elapsed Time:     " << testDur.as_seconds() << " s (" << testDur.as_microseconds() << " us)";
    LOG(Info) << "[Bandwidth] Frequency:        " << iterations / testDur.as_seconds() << " Hz";
    LOG(Info) << "[Bandwidth] Timer Misses:     " << timer.get_misses();
    LOG(Info) << "[Bandwidth] Timer Miss Rate:  " << timer.get_miss_rate();
    LOG(Info) << "[Bandwidth] Timer Wait Ratio: " << timer.get_wait_ratio();

    csv_write_rows("quanser_bandwidth.csv", data);

    return 0;
}

int main(int argc, char* argv[]) {
    MEL_LOG->set_max_severity(Info);
    Options options("options.exe", "Simple Program Demonstrating Options");
    options.add_options()
        ("d,daq",       "The type of DAQ connected to test (q2, q8, or qpid).",   value<std::string>())
        ("f,frequency", "The tested frequency in Hz (default = 1000).",           value<int>())
        ("t,time",      "The tested duration in seconds (default = 10).",         value<int>())
        ("i,input",     "Input channel for loopback (default = 0)",               value<int>())
        ("o,output",    "Output channel for loopback (default = 0)",              value<int>())
        ("r,realtime",  "Enable Windows realtime thread priority.")
        ("h,help",      "Prints helpful information.");

    auto user_input = options.parse(argc, argv);    
    if (user_input.count("help") > 0) { 
        print(options.help());
        return 0;
    }

    int frequency = 1000;
    if (user_input.count("f"))
        frequency = user_input["f"].as<int>();
    int time  = 10;
    if (user_input.count("t"))
        time = user_input["t"].as<int>();
    int input_ch = 0;
    if (user_input.count("i"))
        input_ch = user_input["i"].as<int>();
    int output_ch = 0;
    if (user_input.count("o"))
        output_ch = user_input["o"].as<int>();

    if (user_input.count("r")) {
        enable_realtime();
    }


    print(microseconds);

    if (user_input.count("d")) {
        auto str = user_input["d"].as<std::string>();
        if (str == "q2") {
            Q2Usb q2;
            return run_test(q2, frequency, time, input_ch, output_ch);
        }
        else if (str == "q8") {
            Q8Usb q8;
            return run_test(q8, frequency, time, input_ch, output_ch);
        }
        else if (str == "qpid") {
            QPid qpid;
            return run_test(qpid, frequency, time, input_ch, output_ch);
        }
        else {
            LOG(Error) << "[Bandwidth] DAQ string " << str << " is invalid. Use q2, q8, or qpid";
            print(options.help());
            return 1;
        }
    }
    else {
        LOG(Error) << "[Bandwidth] No DAQ string provided. Enter -d q2 or -d q8 or -d qpid";
        print(options.help());
        return 1;
    }
}
