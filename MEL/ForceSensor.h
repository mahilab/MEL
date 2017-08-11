#pragma once
#include <string>
#include "Daq.h"

namespace mel {

    class ForceSensor {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        ForceSensor();
        ForceSensor(std::string name, std::vector<Daq::Ai> ai_channels);

        // PUBLIC FUNCTIONS
        

        // PUBLIC VARIABLES

        std::string name_;

    private:

        // PRIVATE VARIABLES
        std::vector<Daq::Ai> ai_channels_; // the DAQ analog input channels bound to this sensor

        int num_axes_;


    };
}