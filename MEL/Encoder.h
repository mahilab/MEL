#pragma once
#include "PositionSensor.h"
#include "Daq.h"

namespace mel {

    class Encoder : public PositionSensor {

    public:        

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        Encoder();
        Encoder(std::string name, double counts_per_unit, Daq::Encoder encoder_channel);
        Encoder(std::string name, double counts_per_unit, Daq::Encoder encoder_channel, Daq::EncRate encrate_channel);

        //Encoder& operator= (const Encoder &rhs);

        // PUBLIC FUNCTIONS

        double get_position() override;   // converts encoder count to encoder position in [unit], implicity defined by counts_per_unit_;
        double get_velocity() override;   // converts encoder count rate to encoder velocity in [unit/s], implicity defined by counts_per_unit_;

        void zero();                      // zeros the encoder position to the current location
        void offset(double offset_units); // zeros the encoder offset_units from the current location

        // PUBLIC VARIABLES

        double counts_per_unit_; // the number of counts per one [unit] (e.g. 500 [counts/rev] OR 500  / ( 2 * PI ) [counts/rad])

    private:        

        // PRIVATE VARIABLES

        Daq::Encoder encoder_channel_; // the DAQ encoder channel bound to this Encoder
        Daq::EncRate encrate_channel_; // the DAQ encoder rate channel bound to this Encoder (only used if if velocity_enabled_ = true)

        // STATE VARIABLES

        double count_; // stores the encoder count since the last call to get_position()
        double rate_;  // stores the encoder rate since the last call to get_velocity()

    };
}