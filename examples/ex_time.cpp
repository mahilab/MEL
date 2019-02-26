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

#include <MEL/Core/Time.hpp>
#include <MEL/Core/Clock.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Core/Console.hpp>   // for print
#include <MEL/Utility/System.hpp> // for sleep

using namespace mel;

int main(int argc, char const *argv[])
{

    // Time

    Time t1 = seconds(10);
    Time t2 = milliseconds(10);
    Time t3 = t1 + t2;
    if (t1 > t2)    
        print(t3);

    double t1_s  = t1.as_seconds();
    int32  t2_ms = t2.as_milliseconds();
    int64  t3_us = t3.as_microseconds();

    // Frequency

    Frequency f1 = hertz(1000);
    Frequency f2 = t2.to_frequency();
    print(f1,f2); 

    // Clock

    Clock clock;
    sleep(seconds(1));
    Time time1 = clock.get_elapsed_time();
    sleep(seconds(1));
    Time time2 = clock.restart();
    sleep(seconds(1));
    Time time3 = clock.get_elapsed_time();
    print(time1, time2, time3);

    // Timer

    Timer timer1(seconds(1));
    time1 = timer1.wait();
    time2 = timer1.wait();
    time3 = timer1.wait();
    print(time1, time2, time3);

    Timer hybrid_timer(milliseconds(10), Timer::WaitMode::Hybrid);
    print(hybrid_timer.wait());

    // Time loops

    Time t = Time::Zero;
    Timer timer(hertz(1000));
    while (t < seconds(60)) {
        // code which executes in less than one millisecond
        t = timer.wait();
    }

    



    return 0;
}
