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

#include <MEL/Communications/MelShare.hpp>
#include <MEL/Devices/Windows/Keyboard.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Mechatronics/Limiter.hpp>

using namespace mel;

// Usage:
// Run the example with limiter.scope open in MELScope. Use the up and down
// arrow keys (with the terminal focused) to simulate a commanded value.

int main() {
    // create limiter with continous value of 6, peak value of 18, and
    // time limit of 1 second
    Limiter limiter(6, 18, seconds(1));
    // Create MelShare and data container
    MelShare ms("limiter");
    std::vector<double> data{6, -6, 18, -18, 0, 0, 0, 0};
    data[6] = limiter.get_setpoint();
    // Start control loop
    Timer timer(milliseconds(1), Timer::Sleep);
    while(!Keyboard::is_key_pressed(Key::Escape)) {
        // simulate a command (hold up to increase, down to decrease)
        if (Keyboard::is_key_pressed(Key::Up))
            data[4] += 0.01;
        else if (Keyboard::is_key_pressed(Key::Down))
            data[4] -= 0.01;
        // compute the limited value
        data[5] = limiter.limit(data[4]);
        // get the accumulation
        data[7] = limiter.get_accumulator();
        // write MelShare
        ms.write_data(data);
        // wait
        timer.wait();
    }
    return 0;
}
