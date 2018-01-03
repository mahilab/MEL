#pragma once

#include <MEL/Core/PositionSensor.hpp>
#include <MEL/Core/Daq.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates an optical rotary Encoder
class Encoder : public PositionSensor {

public:

    Encoder();
    Encoder(std::string name, double counts_per_unit, Daq::Encoder encoder_channel);
    Encoder(std::string name, double counts_per_unit, Daq::Encoder encoder_channel, Daq::EncRate encrate_channel);

    /// Converts encoder count to encoder position in [unit], implicity defined by counts_per_unit_
    double get_position() override;

    /// Converts encoder count rate to encoder velocity in [unit/s], implicity defined by counts_per_unit_;
    double get_velocity() override;

    /// Zeros the encoder position to the current location (TODO)
    void zero();

    /// Zeros the encoder offset_units from the current location (TODO)
    void offset(double offset_units);

    /// Gets and returns the encoder count directly
    int32 get_encoder_counts();

    /// Gets and returns the encoder count directly
    double get_encoder_rate();

public:

    double counts_per_unit_; ///< the number of counts per one [unit] (e.g. 500 [counts/rev] OR 500  / ( 2 * PI ) [counts/rad])

private:

    Daq::Encoder encoder_channel_; ///< the DAQ encoder channel bound to this Encoder
    Daq::EncRate encrate_channel_; ///< the DAQ encoder rate channel bound to this Encoder (only used if if velocity_enabled_ = true)

    double count_; ///< stores the encoder count since the last call to get_position()
    double rate_;  ///< stores the encoder rate since the last call to get_velocity()

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
