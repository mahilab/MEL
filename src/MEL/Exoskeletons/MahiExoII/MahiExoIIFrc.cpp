#include "MEL/Exoskeletons/MahiExoII/MahiExoIIFrc.hpp"

namespace mel {

MahiExoIIFrc::MahiExoIIFrc(MeiiConfiguration configuration, MeiiParameters parameters) :
    MahiExoII(configuration, parameters),
    wrist_force_sensor_("wrist_force_sensor", configuration.ai_channels_, parameters.calib_mat_),
    wrist_forces_(std::vector<double>(6, 0.0))
{
    //std::vector<Ai> ai_channels;
    //for (int i = 0; i < 6; ++i) {
    //    ai_channels.push_back(configuration.wrist_force_sensor_[i]);
    //}
    //wrist_force_sensor_ = new AtiMini45("wrist_force_sensor", ai_channels, parameters.calib_mat_);
}

} // namespace mel
