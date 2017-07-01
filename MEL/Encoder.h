#pragma once
#include "PositionSensor.h"
#include "Daq.h"

namespace mel {

    class Encoder : public PositionSensor {

    public:        

        // constructor
        Encoder() {}
        Encoder(uint32 counts_per_rev, Daq::EncoderChannel encoder_channel, Daq::EncRateChannel encrate_channel);

        const uint32 counts_per_rev_ = 0;

        double get_position() override;
        double get_velocity() override; 

    private:

        Daq::EncoderChannel encoder_channel_;
        Daq::EncRateChannel encrate_channel_;

        // state variables
        double count_;
        double rate_;


    };
}