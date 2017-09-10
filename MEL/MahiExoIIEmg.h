#pragma once
#include "MahiExoII.h"
#include "EmgElectrode.h"

namespace mel {

    class MahiExoIIEmg : public virtual MahiExoII {

    public:

        struct Config : public virtual MahiExoII::Config {
            std::array<core::Daq::Ai, 8> emg_; // analog input channels that measure EMG voltages
        };

        struct Params : public virtual MahiExoII::Params {

        };

        // CONSTRUCTOR
        MahiExoIIEmg(Config configuration, Params parameters = Params());


        // PUBLIC FUNCTIONS

        double_vec get_emg_voltages();

        // PUBLIC VARIABLES

        std::vector<core::EmgElectrode*> emg_electrodes_;

    private:

        // PRIVATE VARIABLES

        double_vec emg_voltages_;

    };
}
