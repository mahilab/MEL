#pragma once
#include "Robot.h"

class Encoder {

public:

    // parameters
    const double transmission_;
    const uint counts_per_revolution_;
    const uint quadrature_factor_;
    const uint daq_channel_;
    
    // relatives
    Daq* daq_;
    
    // constructor
    Encoder(double transmission, int counts_per_revolution, int quadrature_factor, Daq* daq, int daq_channel);

    
    // state variables
    double count_;
    double count_rate_;
    
    // getters and setters of state variables
    double get_count();
    double get_count_rate();
    
};

