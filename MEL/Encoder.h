#pragma once
#include "Robot.h"

class Encoder {

public:

    // parameters
    const double transmission_;
    const int counts_per_revolution_;
    const int quadrature_factor_;
    const int daq_channel_;
    
    // relatives
    Daq* daq_;
    
    // constructor
    Encoder(double transmission, int counts_per_revolution, int quadrature_factor, int daq_channel, Daq* daq)
        : transmission_(transmission),
        counts_per_revolution_(counts_per_revolution),
        quadrature_factor_(quadrature_factor),
        daq_channel_(daq_channel),
        daq_(daq) {}
    
    // state variables
    double counts_;
    double velocity_;
    
    // getters and setters of state variables
    double get_counts();
    double get_velocity();
    
};

