#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRioFpga60/MyRio.h"

extern NiFpga_Session myrio_session;

namespace mel {

namespace {

// AO registers
static const std::vector<std::vector<uint32_t>> REGISTERS({
    {AOA_0VAL, AOA_1VAL},
    {AOB_0VAL, AOB_1VAL},
    {AOC_0VAL, AOC_1VAL}
});

// AO weights
static const std::vector<std::vector<double>> WEIGHTS({
    {AOA_0WGHT / 1000000000.0, AOA_1WGHT / 1000000000.0},
    {AOB_0WGHT / 1000000000.0, AOB_1WGHT / 1000000000.0},
    {AOC_0WGHT / 1000000000.0, AOC_1WGHT / 1000000000.0}
});

// AO offsets
static const std::vector<std::vector<double>> OFFSETS({
    {AOA_0OFST / 1000000000.0, AOA_1OFST / 1000000000.0},
    {AOB_0OFST / 1000000000.0, AOB_1OFST / 1000000000.0},
    {AOC_0OFST / 1000000000.0, AOC_1OFST / 1000000000.0}
});

} // namespace

MyRioAO::MyRioAO(MyRio& daq, MyRioConnectorType type, const std::vector<uint32>& channel_numbers) :
  daq_(daq),
  type_(type)
{
    set_name(daq.get_name() + "_AO");
    set_channel_numbers(channel_numbers);
}

// bool MyRioAO::enable() {
//     if (is_enabled())
//         return Device::enable();
//     set_values(enable_values_.get());
//     if (update()) {
//         LOG(Verbose) << "Set " << get_name() << " enable values to " << enable_values_;
//         return Device::enable();
//     }
//     else {
//         LOG(Error) << "Failed to set " << get_name() << " enable values to " << enable_values_;
//         return false;
//     }
// }

// bool MyRioAO::disable() {
//     if (!is_enabled())
//         return Device::disable();
//     set_values(disable_values_.get());
//     if (update()) {
//         LOG(Verbose) << "Set " << get_name() << " disable values to " << disable_values_;
//         return Device::disable();
//     }
//     else {
//         LOG(Error) << "Failed to set " << get_name() << " disable values to " << disable_values_;
//         return false;
//     }
// }

bool MyRioAO::update_channel(uint32 channel_number) {
    if (!daq_.is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }

    NiFpga_Status status;
    uint16_t valueScaled;
    double value = values_[channel_number];

    if (type_ == MyRioConnectorType::MspC)
    {
        value = (value - OFFSETS[type_][channel_number]) / WEIGHTS[type_][channel_number];
        value = (value < INT16_MIN) ? INT16_MIN : value;
        value = (value > INT16_MAX) ? INT16_MAX : value;
        value += (value < 0.0) ? -0.5 : 0.5;
        valueScaled = (uint16_t)((int16_t)(value));
    }
    else
    {
        value = (value - OFFSETS[type_][channel_number]) / WEIGHTS[type_][channel_number] + 0.5;
        value = (value < 0) ? 0 : value;
        value = (value > UINT16_MAX) ? UINT16_MAX : value;
        valueScaled = (uint16_t) value;
    }

    status = NiFpga_WriteU16(myrio_session, REGISTERS[type_][channel_number], valueScaled);
    if (status < 0) {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number;
        return false;
    }

    status = NiFpga_WriteU16(myrio_session, AOSYSGO, 1);
    if (status < 0) {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number;
        return false;
    }


    return true;
}

}  // namespace mel
