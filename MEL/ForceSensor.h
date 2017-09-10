#pragma once
#include "Device.h"
#include "Daq.h"

namespace mel {

    namespace core {

        class ForceSensor : public Device {

        public:

            //---------------------------------------------------------------------
            // CONSTRUCTOR(S) / DESTRUCTOR(S)
            //---------------------------------------------------------------------

            ForceSensor();
            ForceSensor(std::string name, std::vector<Daq::Ai> ai_channels);

            //---------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //---------------------------------------------------------------------

            virtual double_vec get_forces() = 0;

        protected:

            //---------------------------------------------------------------------
            // PROTECTED VARIABLES
            //---------------------------------------------------------------------

            std::vector<Daq::Ai> ai_channels_; // the DAQ analog input channels bound to this sensor

            int num_channels_;

            double_vec forces_;
            double_vec voltages_;

            //---------------------------------------------------------------------
            // PROTECTED FUNCTIONS
            //---------------------------------------------------------------------

            double_vec get_voltages();

        };

    }

}