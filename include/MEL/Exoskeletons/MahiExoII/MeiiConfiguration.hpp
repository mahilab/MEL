#pragma once

#include <MEL/Daq/Daq.hpp>
#include <MEL/Daq/Encoder.hpp>
#include <MEL/Daq/Velocity.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>
#include <MEL/Daq/Watchdog.hpp>
#include <MEL/Core/Amplifier.hpp>
#include <vector>

namespace mel {

    //==============================================================================
    // FORWARD DECLARATIONS
    //==============================================================================

    class MahiExoII;

    //==============================================================================
    // CLASS DECLARATION
    //==============================================================================

    /// Encapsulates the hardware configuration for a MahiExoII
    class MeiiConfiguration {

    public:

        /// Constructor for standard configuration
        MeiiConfiguration(
            Daq& daq,
            Watchdog& watchdog,
            const std::vector<Encoder::Channel>& encoder_channels,
            const std::vector<Velocity::Channel>& velocity_channels,
            const std::vector<Amplifier>& amplifiers);

        /// Constructor for EMG or Force Sensor configuration
        MeiiConfiguration(
            Daq& daq,
            Watchdog& watchdog,
            const std::vector<Encoder::Channel>& encoder_channels,
            const std::vector<Velocity::Channel>& velocity_channels,
            const std::vector<Amplifier>& amplifiers,
            const std::vector<AnalogInput::Channel>& ai_channels);



    private:

        friend class MahiExoII;
        friend class MahiExoIIEmg;
        friend class MahiExoIIFrc;

        Daq&                                  daq_;                ///< DAQ controlling the MahiExoII
        Watchdog&                             watchdog_;           ///< watchdog the MahiExoII is guarded by
        std::vector<Encoder::Channel>         encoder_channels_;   ///< encoder channels that measure motor positions
        std::vector<Velocity::Channel>        velocity_channels_;  ///< encoder channels that measure motor velocities
        std::vector<Amplifier>                amplifiers_;         ///< amplifiers used to control robot motors
        std::vector<AnalogInput::Channel>     ai_channels_;        ///< analog input channels that measure EMG

    };

} // namespace mel

  //==============================================================================
  // CLASS DOCUMENTATION
  //==============================================================================