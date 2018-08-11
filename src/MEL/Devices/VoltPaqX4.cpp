#include <MEL/Devices/VoltPaqX4.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

VoltPaqX4::VoltPaqX4(const std::vector<DigitalOutput::Channel>& enable_channels,
    const std::vector<AnalogOutput::Channel>&  command_channels,
    const std::vector<DigitalInput::Channel>&  fault_channels,
    const std::vector<AnalogInput::Channel>&   sense_channels)
{
    std::size_t num_amps = enable_channels.size();
    if (command_channels.size() == num_amps && fault_channels.size() == num_amps && sense_channels.size() == num_amps) {
        for (std::size_t i = 0; i < num_amps; ++i)
            amplifiers.push_back(Amplifier("voltpaq-x4_" + stringify(i), High, enable_channels[i], 1.0, command_channels[i], Limiter(), High, fault_channels[i], 1.0, sense_channels[i]));
    }
    else {
        LOG(Error) << "Unequal number of channels provided to VoltPaqX4 constructor. No amplifiers were intialized.";
    }
}

bool VoltPaqX4::on_enable() {
    bool success = true;
    for (std::size_t i = 0; i < amplifiers.size(); ++i) {
        if(!amplifiers[i].enable())
            success = false;
    }
    return success;
}

bool VoltPaqX4::on_disable() {
    bool success = true;
    for (std::size_t i = 0; i < amplifiers.size(); ++i) {
        if(!amplifiers[i].disable())
            success = false;
    }
    return success;
}

} // namespace mel
