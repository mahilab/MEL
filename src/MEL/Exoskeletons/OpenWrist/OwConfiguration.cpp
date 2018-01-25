#include <MEL/Exoskeletons/OpenWrist/OwConfiguration.hpp>
#ifdef MEL_INCLUDE_QUANSER
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#endif

namespace mel {

//==============================================================================
// CLASS DEFINTIONS
//==============================================================================

OwConfiguration::OwConfiguration(
    Daq& daq,
    Watchdog& watchdog,
    const std::vector<Amplifier::TtlLevel>& enable_levels,
    const std::vector<DigitalOutput::Channel>& enable_channels,
    const std::vector<double>& command_gains,
    const std::vector<AnalogOutput::Channel>& command_channels,
    const std::vector<Limiter>& amp_current_limiters,
    const std::vector<Amplifier::TtlLevel>& fault_levels,
    const std::vector<DigitalInput::Channel>& fault_channels,
    const std::vector<double>& sense_gains,
    const std::vector<AnalogInput::Channel>& sense_channels,
    const std::vector<Encoder::Channel>& encoder_channels,
    const std::vector<Velocity::Channel>& velocity_channels)
    : daq_(daq),
      watchdog_(watchdog),
      encoder_channels_(encoder_channels),
      velocity_channels_(velocity_channels) {
    for (std::size_t i = 0; i < 3; ++i) {
        amplifiers_.push_back(
            Amplifier("ow_amp_" + std::to_string(i), enable_levels[i],
                      enable_channels[i], command_gains[i], command_channels[i],
                      amp_current_limiters[i], fault_levels[i],
                      fault_channels[i], sense_gains[i], sense_channels[i]));
    }
}

OwConfiguration::OwConfiguration(
    Daq& daq,
    Watchdog& watchdog,
    const std::vector<Encoder::Channel>& encoder_channels,
    const std::vector<Velocity::Channel>& velocity_channels,
    const std::vector<Amplifier>& amplifiers)
    : daq_(daq),
      watchdog_(watchdog),
      encoder_channels_(encoder_channels),
      velocity_channels_(velocity_channels),
      amplifiers_(amplifiers) {}

}  // namespace mel

#ifdef MEL_INCLUDE_QUANSER
OwConfiguration::OwConfiguration(Q8Usb& q8_usb)
    : OwConfiguration(
          q8_usb,
          q8_usb.watchdog,
          std::vector<Amplifier::TtlLevel>(3, Amplifier::TtlLevel::High),
          q8_usb.digital_output[{0, 1, 2}],
          {1.0, 1.0, 1.0},
          q8_usb.analog_output[{0, 1, 2}],
          {Limiter(), Limiter(), Limiter()},
          std::vector<Amplifier::TtlLevel>(3, Amplifier::TtlLevel::High),
          q8_usb.digital_input[{0, 1, 2}],
          {1.0, 1.0, 1.0},
          q8_usb.analog_input[{0, 1, 2}],
          q8_usb.encoder[{0, 1, 2}],
          q8_usb.velocity[{0, 1, 2}]) {}
#endif
