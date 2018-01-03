#pragma once
#include <MEL/Core/Device.hpp>
#include <MEL/Daq/InputModule.hpp>

namespace mel {

class ForceSensor : public Device {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    ForceSensor();
    ForceSensor(std::string name, std::vector<InputModule<voltage>::Channel> ai_channels);

    //---------------------------------------------------------------------
    // PUBLIC FUNCTIONS
    //---------------------------------------------------------------------

    virtual std::vector<double> get_forces() = 0;

protected:

    //---------------------------------------------------------------------
    // PROTECTED VARIABLES
    //---------------------------------------------------------------------

    std::vector<InputModule<voltage>::Channel> ai_channels_; // the DAQ analog input channels bound to this sensor

    int num_channels_;

    std::vector<double> forces_;
    std::vector<double> voltages_;

    //---------------------------------------------------------------------
    // PROTECTED FUNCTIONS
    //---------------------------------------------------------------------

    std::vector<double> get_voltages();

};

}
