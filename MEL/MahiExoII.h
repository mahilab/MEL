#pragma once
#include "Robot.h"

class MahiExoII : public Robot {

    // constructor
    MahiExoII(Daq* e0_daq, uint e0_chan,
        Daq* e1_daq, uint e1_chan,
        Daq* ao0_daq, uint ao0_chan,
        Daq* ao1_daq, uint ao1_chan,
        Daq* ai0_daq, uint ai0_chan,
        Daq* ai1_daq, uint ai1_chan,
        Daq* do0_daq, uint do0_chan,
        Daq* do1_daq, uint do1_chan);

    // parameters
    Daq* e0_daq_;
    Daq* e1_daq_;
    Daq* ao0_daq_;
    Daq* ao1_daq_;
    Daq* ai0_daq_;
    Daq* ai1_daq_;
    Daq* do0_daq_;
    Daq* do1_daq_;
    const uint e0_chan_;
    const uint e1_chan_;
    const uint ao0_chan_;
    const uint ao1_chan_;
    const uint ai0_chan_;
    const uint ai1_chan_;
    const uint do0_chan_;
    const uint do1_chan_;

    // references
    

};