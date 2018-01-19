#include <MEL/Exoskeletons/MahiExoII/MeiiConfiguration.hpp>

namespace mel {

    //==============================================================================
    // CLASS DEFINTIONS
    //==============================================================================

    MeiiConfiguration::MeiiConfiguration(
        Daq& daq,
        Watchdog& watchdog,
        const std::vector<Output<logic>::Channel>& enable_channels,
        const std::vector<Output<voltage>::Channel>& command_channels,
        const std::vector<Input<voltage>::Channel>& sense_channels,
        const std::vector<Encoder::Channel>& encoder_channels,
        const std::vector<Velocity::Channel>& velocity_channels,
        const std::vector<double>& amplifier_gains) :
        daq_(daq),
        watchdog_(watchdog),
        enable_channels_(enable_channels),
        command_channels_(command_channels),
        sense_channels_(sense_channels),
        encoder_channels_(encoder_channels),
        velocity_channels_(velocity_channels),
        amplifier_gains_(amplifier_gains)
    {
    }


} // namespace mel