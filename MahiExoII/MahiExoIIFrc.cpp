#include "MahiExoIIFrc.h"

MahiExoIIFrc::MahiExoIIFrc(Config configuration, Params parameters) :
    MahiExoII(configuration,parameters)
{
    std::vector<mel::Daq::Ai> ai_channels;
    for (int i = 0; i < 6; ++i) {
        ai_channels.push_back(configuration.wrist_force_sensor_[i]);
    }
    wrist_force_sensor_ = new mel::ForceSensor("wrist_force_sensor", ai_channels);
}