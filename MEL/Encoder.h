#pragma once
#include "PositionSensor.h"
#include "Daq.h"

namespace mel {

    class Encoder : public PositionSensor {

    public:        

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        Encoder();
        Encoder(uint32 counts_per_unit, Daq::EncoderChannel encoder_channel);
        Encoder(uint32 counts_per_unit, Daq::EncoderChannel encoder_channel, Daq::EncRateChannel encrate_channel);

        // PUBLIC FUNCTIONS

        double get_position() override; // converts encoder count to encoder position in [unit], implicity defined by counts_per_unit_;
        double get_velocity() override; // converts encoder count rate to encoder velocity in [unit/s], implicity defined by counts_per_unit_;

        void zero(); // zeros the encoder counts to the current location
        void offset(uint32 offset_count);
        void offset(double offset_units);

        // PUBLIC VARIABLES

        const uint32 counts_per_unit_; // the number of counts per one [unit] (e.g. 500 [counts/rev] OR 500  / ( 2 * PI ) [counts/rad])

    private:        

        // PRIVATE VARIABLES

        Daq::EncoderChannel encoder_channel_;
        Daq::EncRateChannel encrate_channel_;

        // STATE VARIABLES

        double count_;
        double rate_;


    };
}