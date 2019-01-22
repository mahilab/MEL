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

#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main(int argc, char* argv[]) {

    // register CTRL-C handler
    register_ctrl_handler(handler);

    MelShare q8_ai("q8_ai");
    MelShare q8_di("q8_di");
    MelShare q8_enc("q8_enc");
    MelShare q8_vel("q8_vel");

    std::vector<double> q8_ai_data(8);
    std::vector<double> q8_di_data(8);
    std::vector<double> q8_enc_data(8);
    std::vector<double> q8_vel_data(8);

    Q8Usb q8;
    q8.open();

    // create waveforms for looping
    Waveform sinwave(Waveform::Sin, seconds(1));
    Waveform coswave(Waveform::Cos, seconds(1));
    Waveform sawwave(Waveform::Sawtooth, seconds(1));
    Waveform triwave(Waveform::Triangle, seconds(1));
    Waveform sqrwave1(Waveform::Square, seconds(1), 0.5, 0.5);
    Waveform sqrwave2(Waveform::Square, seconds(2), 0.5, 0.5);
    Waveform sqrwave3(Waveform::Square, seconds(3), 0.5, 0.5);
    Waveform sqrwave4(Waveform::Square, seconds(4), 0.5, 0.5);


    Timer timer(hertz(1000));
    Time t;

    // enable Q8Usb
    q8.enable();

    while (!stop) {
        // update Q8Usb AI, DI, and encoder
        q8.update_input();
        // get current time
        t = timer.get_elapsed_time();
        // write input to MelShares for scoping
        for (uint32 i = 0; i < 8; i++)
        {
            q8_ai_data[i]  = q8.AI[i];
            q8_di_data[i]  = (double)q8.DI[i];
            q8_enc_data[i] = (double)q8.encoder[i];
            q8_vel_data[i] = q8.encoder[i].get_value_per_sec();
        }

        q8_ai.write_data(q8_ai_data);
        q8_di.write_data(q8_di_data);
        q8_enc.write_data(q8_enc_data);
        q8_vel.write_data(q8_vel_data);

        // set AO
        q8.AO[0] = sinwave(t);
        q8.AO[1] = coswave(t);
        q8.AO[2] = sawwave(t);
        q8.AO[3] = triwave(t);
        q8.AO[4] = sinwave(t)*10;
        q8.AO[5] = coswave(t)*10;
        q8.AO[6] = sawwave(t)*10;
        q8.AO[7] = triwave(t)*10;

        // set DO
        q8.DO[0] = sqrwave1(t) == 0 ? Low : High;
        q8.DO[1] = sqrwave2(t) == 0 ? Low : High;
        q8.DO[2] = sqrwave3(t) == 0 ? Low : High;
        q8.DO[3] = sqrwave4(t) == 0 ? Low : High;
        q8.DO[4] = sqrwave1(t) == 0 ? High : Low;
        q8.DO[5] = sqrwave2(t) == 0 ? High : Low;
        q8.DO[6] = sqrwave3(t) == 0 ? High : Low;
        q8.DO[7] = sqrwave4(t) == 0 ? High : Low;

        // update and wait
        q8.update_output();
        timer.wait();

    }

    return 0;
}
