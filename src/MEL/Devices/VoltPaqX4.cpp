#include <MEL/Devices/VoltPaqX4.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

VoltPaqX4::VoltPaqX4(const std::vector<DigitalOutput::Channel>& enable_channels,
    const std::vector<AnalogOutput::Channel>&  command_channels,
    const std::vector<DigitalInput::Channel>&  fault_channels,
    const std::vector<AnalogInput::Channel>&   sense_channels)
{
    std::size_t num_amps = enable_channels.size();
    if (command_channels.size() == num_amps && fault_channels.size() == num_amps && sense_channels.size() == num_amps) {
        for (int i = 0; i < num_amps; ++i)
            amplifiers.push_back(Amplifier("voltpaq-x4_" + stringify(i), Amplifier::High, enable_channels[i], 1.0, command_channels[i], Limiter(), Amplifier::High, fault_channels[i], 1.0, sense_channels[i]));
    }
    else {
        print("ERROR: Unequal number of channels provided to VoltPaqX4 constructor. No amplifiers were intialized.");
    }
}

bool VoltPaqX4::enable() {
    for (std::size_t i = 0; i < amplifiers.size(); ++i)
        amplifiers[i].enable();
    return Device::enable();        
}

bool VoltPaqX4::disable() {
    for (std::size_t i = 0; i < amplifiers.size(); ++i)
        amplifiers[i].disable();
    return Device::enable();
}

} // namespace mel