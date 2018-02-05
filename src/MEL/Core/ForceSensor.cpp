#include <MEL/Core/ForceSensor.hpp>

namespace mel {

ForceSensor::ForceSensor() :
    Device("force_sensor"),
    num_channels_(1),
    voltages_({ 0 })
{ }

ForceSensor::ForceSensor(std::string name, std::vector<Input<Voltage>::Channel> ai_channels) :
    Device("ForceSensor::" + name),
    num_channels_(static_cast<int>(ai_channels.size())),
    voltages_(std::vector<double>(ai_channels.size(), 0))
{
    for (int i = 0; i < num_channels_; ++i) {
        ai_channels_.push_back(ai_channels[i]);
    }
}

std::vector<double> ForceSensor::get_voltages() {
    for (int i = 0; i < num_channels_; ++i) {
        voltages_[i] = ai_channels_[i].get_value();
    }
    return voltages_;
}

} // namespace mel
