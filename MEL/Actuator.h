#pragma once
#include "Daq.h"

class Actuator {

public:

    // parameters
    const double transmission_;
    const double kt_;
    const double amp_gain_;
    const double current_limit_;
    const uint ao_channel_;
    const uint do_channel_;

    // relatives
    Daq* daq_;

    // constructor
    Actuator(double transmission, double kt, double amp_gain, double current_limit, Daq* daq, uint ao_channel, uint do_channel)
        : transmission_(transmission),
        kt_(kt),
        amp_gain_(amp_gain),
        current_limit_(current_limit),
        ao_channel_(ao_channel),
        do_channel_(do_channel),
        daq_(daq) {}
    
    // state variables
    double torque_;
    double current_;
    double command_;
    
    // setters of state variables
    void set_torque(double torque);
    void set_current(double current);
    void set_command(double voltage);
    
    void enable();
    void disable();
};