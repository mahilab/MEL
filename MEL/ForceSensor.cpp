#include "ForceSensor.h"

namespace mel {

    ForceSensor::ForceSensor() :
        name_("no_name"),
        num_axes_(1)
    { }

    ForceSensor::ForceSensor(std::string name, std::vector<Daq::Ai> ai_channels) :
        name_(name),
        num_axes_(ai_channels.size())
    {
        for (int i = 0; i < num_axes_; ++i) {
            ai_channels_.push_back(ai_channels[i]);
        }
    }


}