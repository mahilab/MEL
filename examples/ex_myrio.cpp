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

using namespace mel;
using namespace std;

int main(int argc, char** argv) {

    MelNet mn(55002, 55001, "192.168.1.7");

    MyRio myrio("myrio");
    myrio.open();
    myrio.enable();
    Waveform sinwave(Waveform::Sin, seconds(1), 5);

    std::vector<double> data(2);

    Timer timer(hertz(1000));
    Time t;
    while ((t = timer.get_elapsed_time()) < seconds(60)) {
        myrio.update_input();

        data[0] = sinwave(t);
        myrio.C.AO[0].set_value(data[0]);
        data[1] = myrio.C.AI[0];

        print(myrio.C.encoder[0].get_value());



        myrio.update_output();
        timer.wait();
    }

}
