#include <MEL/Exoskeletons/MahiExoII/MeiiConfiguration.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINTIONS
//==============================================================================

MeiiConfiguration::MeiiConfiguration(
    Daq& daq,
    Watchdog& watchdog,
    const std::vector<Encoder::Channel>& encoder_channels,
    const std::vector<Velocity::Channel>& velocity_channels,
    const std::vector<Amplifier>& amplifiers) :
    daq_(daq),
    watchdog_(watchdog),
    encoder_channels_(encoder_channels),
    velocity_channels_(velocity_channels),
    amplifiers_(amplifiers)
{
}

MeiiConfiguration::MeiiConfiguration(
    Daq& daq,
    Watchdog& watchdog,
    const std::vector<Encoder::Channel>& encoder_channels,
    const std::vector<Velocity::Channel>& velocity_channels,
    const std::vector<Amplifier>& amplifiers,
    const std::vector<AnalogInput::Channel>& ai_channels) :
    daq_(daq),
    watchdog_(watchdog),
    encoder_channels_(encoder_channels),
    velocity_channels_(velocity_channels),
    amplifiers_(amplifiers),
    ai_channels_(ai_channels)
{
}


} // namespace mel