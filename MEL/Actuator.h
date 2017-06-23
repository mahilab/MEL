#pragma once
#include "Daq.h"

class Actuator {

public:

    // parameters
    const double transmission_;
    const double kt_;
    const double amp_gain_;
    const double current_limit_;
    const int daq_channel_;

    // relatives
    Daq* daq_;

    // constructor
    Actuator(double transmission, double kt, double amp_gain, double current_limit, int daq_channel, Daq* daq)
        : transmission_(transmission),
        kt_(kt),
        amp_gain_(amp_gain),
        current_limit_(current_limit),
        daq_channel_(daq_channel),
        daq_(daq) {}
    
    // state variables
    double torque_;
    double current_;
    double command_;
    
    // setters of state variables
    void set_torque(double torque);
    void set_current(double current);
    void set_command(double voltage);
    
};