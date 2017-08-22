#include "ForceSensor.h"

namespace mel {

    ForceSensor::ForceSensor() :
        Device("force_sensor"),
        num_channels_(1),
        voltages_({ 0 })
    { }

    ForceSensor::ForceSensor(std::string name, std::vector<Daq::Ai> ai_channels) :
        Device(name),
        num_channels_(static_cast<int>(ai_channels.size())),
        voltages_(double_vec(ai_channels.size(),0))
    {
        for (int i = 0; i < num_channels_; ++i) {
            ai_channels_.push_back(ai_channels[i]);
        }
    }

    double_vec ForceSensor::get_voltages() {

        for (int i = 0; i < num_channels_; ++i) {
            voltages_[i] = ai_channels_[i].get_voltage();
        }

        return voltages_;
    }

}