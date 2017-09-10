#include "MahiExoIIFrc.h"

namespace mel {

    MahiExoIIFrc::MahiExoIIFrc(Config configuration, Params parameters) :
        MahiExoII(configuration, parameters)
    {
        std::vector<core::Daq::Ai> ai_channels;
        for (int i = 0; i < 6; ++i) {
            ai_channels.push_back(configuration.wrist_force_sensor_[i]);
        }
        wrist_force_sensor_ = new hdw::AtiMini45("wrist_force_sensor", ai_channels, parameters.calib_mat_);
    }
}