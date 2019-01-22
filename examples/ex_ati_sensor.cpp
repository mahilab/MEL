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
#include <MEL/Core/Timer.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Devices/AtiSensor.hpp>
#include <MEL/Devices/Windows/Keyboard.hpp>

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

    // create and open Q8
    Q8Usb q8;
    q8.open();

    // create ATI sensor
    AtiSensor ati;
    ati.set_channels(q8.AI[{0, 1, 2, 3, 4, 5}]);
    ati.load_calibration("FT12345.cal");

    // alternatively, the calibration can set explicitly
    // AtiSensor::Calibration cal;
    // cal.Fx = { 0.01517,   0.01800, -0.02466,  -1.69832,   0.05208,   1.68355 };
    // cal.Fy = { 0.07231,   2.01105, -0.02110,  -0.95833,  -0.05133,  -0.99616 };
    // cal.Fz = { 1.86652,  -0.03457,  1.90079,  -0.02482,   1.89478,  -0.03528 };
    // cal.Tx = { 0.37651,   12.19008, 10.54613, -5.96091,  -10.70737, -5.83644 };
    // cal.Ty = { -12.07505, 0.17459,  6.36129,   10.18467,  5.70347, -10.34754 };
    // cal.Tz = { 0.09187,   7.34562,  0.09166,   6.92461,   0.52790,   7.07804 };
    // ati.set_calibration(cal);

    // make MelShares for scoping
    MelShare ms_volts("ati_volts");
    MelShare ms_force("ati_force");
    MelShare ms_torque("ati_torque");

    // enable Q8Usb
    q8.enable();

    // make timer
    Timer timer(hertz(1000));

    // start sampling loop
    while (!stop) {

        // update input
        q8.AI.update();

        // zero ATI if user presses Z key
        if (Keyboard::is_key_pressed(Key::Z))
            ati.zero();

        // write voltages, forces, and torques to MelShares for scoping
        ms_volts.write_data({ q8.AI[0], q8.AI[1], q8.AI[2], q8.AI[3], q8.AI[4], q8.AI[5] });
        ms_force.write_data({ ati.get_force(AxisX), ati.get_force(AxisY), ati.get_force(AxisZ) });
        ms_torque.write_data(ati.get_torques());

        // wait timer
        timer.wait();
    }

    // disable and close Q8
    q8.disable();
    q8.close();

    return 0;

}
