#pragma once
#include "Device.h"
#include "Daq.h"

namespace mel {

    namespace core {

        class EmgElectrode : public Device {

        public:

            //---------------------------------------------------------------------
            // CONSTRUCTOR(S) / DESTRUCTOR(S)
            //---------------------------------------------------------------------

            EmgElectrode();
            EmgElectrode(std::string name, Daq::Ai ai_channel);

            //---------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //---------------------------------------------------------------------

            double get_voltage();

        private:

            //---------------------------------------------------------------------
            // PRIVATE VARIABLES
            //---------------------------------------------------------------------

            Daq::Ai ai_channel_; // the DAQ analog input channel bound to this electrode

            double voltage_;

        };

    }

}