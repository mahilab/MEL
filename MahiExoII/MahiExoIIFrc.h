#pragma once
#include "MahiExoII.h"
#include "mahiexoii_util.h"
#include "ForceSensor.h"

class MahiExoIIFrc : public MahiExoII {

public:

    struct Config : public MahiExoII::Config {
        std::array<mel::Daq::Ai,6> wrist_force_sensor_; // analog input channels that measure force/torque
    };

    struct Params : public MahiExoII::Params {
    };

    // CONSTRUCTOR
    MahiExoIIFrc(Config configuration, Params parameters = Params());


    // PUBLIC VARIABLES
    
    mel::ForceSensor* wrist_force_sensor_;

};