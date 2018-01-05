#pragma once

#include <MEL/Daq/Daq.hpp>
#include <MEL/Daq/Encoder.hpp>
#include <MEL/Daq/Velocity.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>
#include <MEL/Daq/Watchdog.hpp>
#include <vector>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class OpenWrist;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates the hardware configuration for an OpenWrist
class OwConfiguration {

public:

    /// Default constructor
    OwConfiguration(
        Daq& daq, 
        Watchdog& watchdog,
        const std::vector<Output<logic>::Channel>& enable_channels,
        const std::vector<Output<voltage>::Channel>& command_channels,
        const std::vector<Input<voltage>::Channel>& sense_channels,
        const std::vector<Encoder::Channel>& encoder_channels_,
        const std::vector<Velocity::Channel>& velocity_channels_,
        const std::vector<double>& amplifier_gains);

private:

    friend class OpenWrist;

    Daq&                                  daq_;                ///< DAQ controlling the OpenWrist
    Watchdog&                             watchdog_;           ///< watchdog the OpenWrist is guarded by
    std::vector<Output<logic>::Channel>   enable_channels_;    ///< digital output channels that enable motors
    std::vector<Output<voltage>::Channel> command_channels_;   ///< analog output channels that command motor currents
    std::vector<Input<voltage>::Channel>  sense_channels_;     ///< analog input channels that sense motor current
    std::vector<Encoder::Channel>         encoder_channels_;   ///< encoder channels that measure motor positions
    std::vector<Velocity::Channel>        velocity_channels_;  ///< encoder channels that measure motor velocities
    std::vector<double>                   amplifier_gains_;    ///< motor aplifier gains

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================