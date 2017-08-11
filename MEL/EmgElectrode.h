#pragma once
#include <string>
#include "Daq.h"

namespace mel {

    class EmgElectrode {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        EmgElectrode();
        EmgElectrode(std::string name, Daq::Ai ai_channel);

        // PUBLIC FUNCTIONS
        double get_voltage();

        // PUBLIC VARIABLES

        std::string name_;

    private:

        // PRIVATE VARIABLES
        
        Daq::Ai ai_channel_; // the DAQ analog input chanel bound to this electrode

        double voltage_;

    };
}