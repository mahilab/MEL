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
        
        virtual double_vec get_forces() = 0;

        // PUBLIC VARIABLES

        std::string name_;

    protected:

        // PROTECTED VARIABLES
        std::vector<Daq::Ai> ai_channels_; // the DAQ analog input channels bound to this sensor

        int num_channels_;

        double_vec forces_;
        double_vec voltages_;

        // PROTECTED FUNCTIONS
        double_vec get_voltages();

    };
}