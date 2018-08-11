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

#include <MEL/Daq/VirtualDaq.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Math/Random.hpp>

using namespace mel;

Voltage my_ai_source(Time t) {
    Waveform tri(Waveform::Triangle, seconds(1));
    return tri.evaluate(t);
}

int main(int argc, char* argv[]) {

    // melshare for plotting
    MelShare ms("virt_daq_test");

    // create virtual daq and open
    VirtualDaq daq("virt_daq");
    daq.open();

    // set custom source for AI channels 0 and 1 (default is a sinwave)
    daq.AI.sources[0] = my_ai_source;                           // from function
    daq.AI.sources[1] = [](Time t){ return random(-1.0,1.0); }; // from lambda

    daq.enable();

    Timer timer(hertz(1000));
    while(true) {
        daq.update_input();
        ms.write_data({
            daq.AI[0],
            daq.AI[1],
            daq.AI[2],
            (double)daq.DI[0],
            (double)daq.encoder[0].get_value(),
            daq.encoder[0].get_position() });
        daq.update_output();
        timer.wait();
    }

    return 0;
}
