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

#include <MEL/Mechatronics/PidController.hpp>
#include <MEL/Mechatronics/PdController.hpp>
#include <MEL/Communications/MelShare.hpp>

using namespace mel;

// Simulated system to control
class MassSpringDamper {
public:
    // Constructor
    MassSpringDamper(double x_0 = 0, double xd_0 = 0) :
        x(x_0), xd(xd_0), xdd(0)
    {
        xdd_xd.set_init(xd);
        xd_x.set_init(x);
    }
    // Propogate MSD system
    void update(double F, Time t) {
        xdd = (F - b * xd - k * x) / m;
        xd  = xdd_xd.update(xdd, t); 
        x   = xd_x.update(xd, t);
    }
    // Properties
    double m = 10;  // mass, kg    
    double k = 20; // spring, N/m
    double b = 30; // damping, Ns/m
    // State
    double x; // position, m
    double xd; // velocity, m/s
    double xdd; // acceleration, m/s^2
private:
    // Integrators
    Integrator xdd_xd, xd_x;    
};

int main() {
    // MelShare for plotting
    MelShare ms("pid");
    // Timer
    Timer timer(hertz(1000));
    // Time point
    Time t;    
    // MSD system to simulate and control
    MassSpringDamper msd(-10.0);
    // PID controller
    PidController pid(1000, 50, 100);    
    // simulate for 10 seconds
    while ((t = timer.get_elapsed_time()) < seconds(10)) {
        // calculate control effort to move mass to 10 m
        double F = pid.calculate(10, msd.x, t);
        // update MSD system
        msd.update(F, t);    
        // write data to MelShare for plotting
        ms.write_data({ msd.x, msd.xd, msd.xdd });
        // wait timer
        timer.wait();
    }
    return 0;
}