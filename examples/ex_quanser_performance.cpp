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
int analog_test(TDaq& daq, int frequency, int time, int ai, int ao, bool save) {

    if (!(daq.open() && daq.enable())) 
        return 1;

    int iterations = frequency * time;
    std::vector<std::array<double,3>> data(iterations);
    Waveform sinwave(Waveform::Sin, hertz(100), 10);
    Timer timer(hertz(frequency), Timer::Busy, false);
    Time t = Time::Zero;

    LOG(Info) << "---Starting Test---";
    LOG(Info) << "Frequency:        " << frequency << " Hz";
    LOG(Info) << "Duration:         " << time << " s";
    LOG(Info) << "Iterations:       " << iterations;
    LOG(Info) << "AI/AO Channels:   " << ai << "/" << ao;

    for (int i = 0; i < iterations; ++i) {
        daq.AI.update();
        data[i][0] = t.as_seconds();
        data[i][1] = daq.AI[ai];
        data[i][2] = sinwave(t);
        daq.AO[ao] = data[i][2];
        daq.AO.update();
        t = timer.wait();
    }

    // sum repeated measures
    int repeats = 0;
    for (int i = 1; i < iterations; ++i) {
        if (data[i][1] == data[i-1][1])
            repeats++;
    }

    LOG(Info) << "---Test Completed---";
    LOG(Info) << "Repeats:          " << repeats;
    LOG(Info) << "Repeat Rate:      " << (double)repeats / (double)(iterations - 1);
    LOG(Info) << "Elapsed Time:     " << t.as_seconds() << " s (" << t.as_microseconds() << " us)";
    LOG(Info) << "Frequency:        " << iterations / t.as_seconds() << " Hz";
    LOG(Info) << "Timer Misses:     " << timer.get_misses();
    LOG(Info) << "Timer Miss Rate:  " << timer.get_miss_rate();
    LOG(Info) << "Timer Wait Ratio: " << timer.get_wait_ratio();

    if (save) {
        LOG(Info) << "Saving Test Data ...";
        Timestamp stamp;
        std::vector<std::string> header = {"Time", "AI_" + std::to_string(ai), "AO_" + std::to_string(ao)};
        std::string filename = daq.get_name() + "_f" + std::to_string(frequency) + "_t" + std::to_string(time) + "_" + stamp.hh_mm_ss_mmm() + ".csv";
        csv_write_rows(filename, header);
        csv_append_rows(filename, data);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    MEL_LOG->set_max_severity(Info);
    Options options("quanser_performance.exe", "Utility Program to Evaluate Quanser DAQ Performance");
    options.add_options()
        ("q", "The type of Quanser DAQ connected to test (q2, q8, or qpid).", value<std::string>())
        ("f", "The target frequency in Hz (default = 1000).",                 value<int>())
        ("t", "The test duration in seconds (default = 10).",                 value<int>())
        ("i", "Input channel for loopback (default = 0)",                     value<int>())
        ("o", "Output channel for loopback (default = 0)",                    value<int>())
        ("r", "Enable Windows realtime thread priority.")
        ("s", "Save test data.")
        ("h", "Prints helpful information.");

    auto user_input = options.parse(argc, argv);    
    if (user_input.count("h") > 0) { 
        print(options.help());
        return 0;
    }

    int frequency = 1000;
    if (user_input.count("f"))
        frequency = user_input["f"].as<int>();
    int time  = 10;
    if (user_input.count("t"))
        time = user_input["t"].as<int>();
    int ai = 0;
    if (user_input.count("i"))
        ai = user_input["i"].as<int>();
    int ao = 0;
    if (user_input.count("o"))
        ao = user_input["o"].as<int>();

    if (user_input.count("r")) {
        if (enable_realtime())
            LOG(Info) << "Enabled Windows Realtime thread priority";
    }

    bool save = user_input.count("s") > 0;

    if (user_input.count("d")) {
        auto str = user_input["d"].as<std::string>();
        if (str == "q2") {
            Q2Usb q2;
            return analog_test(q2, frequency, time, ai, ao, save);
        }
        else if (str == "q8") {
            Q8Usb q8;
            return analog_test(q8, frequency, time, ai, ao, save);
        }
        else if (str == "qpid") {
            QPid qpid;
            return analog_test(qpid, frequency, time, ai, ao, save);
        }
        else {
            LOG(Error) << "DAQ string " << str << " is invalid. Use q2, q8, or qpid";
            print(options.help());
            return 1;
        }
    }
    else {
        LOG(Error) << "No DAQ string provided. Enter -q q2 or -d q8 or -d qpid";
        print(options.help());
        return 1;
    }
}
