#pragma once
#include "MahiExoII.h"
#include "EmgElectrode.h"

class MahiExoIIEmg : public MahiExoII {

public:

    struct Config : public MahiExoII::Config {
        std::array<mel::Daq::Ai, 8> emg_; // analog input channels that measure EMG voltages
    };

    struct Params : public MahiExoII::Params {

    };

    // CONSTRUCTOR
    MahiExoIIEmg(Config configuration, Params parameters = Params());


    // PUBLIC FUNCTIONS

    mel::double_vec get_emg_voltages();

    // PUBLIC VARIABLES

    std::vector<mel::EmgElectrode*> emg_electrodes_;

private:

    // PRIVATE VARIABLES

    mel::double_vec emg_voltages_;

};
