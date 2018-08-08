#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Daq/NI/MyRio/MyRioAI.hpp>
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRio.h"

extern NiFpga_Session myrio_session;

namespace mel {

// AI registers
static const std::vector<std::vector<uint32_t>> REGISTERS({
    {AIA_0VAL, AIA_1VAL, AIA_2VAL, AIA_3VAL},
    {AIB_0VAL, AIB_1VAL, AIB_2VAL, AIB_3VAL},
    {AIC_0VAL, AIC_1VAL}
});

// AI weights
static const std::vector<std::vector<double>> WEIGHTS({
    {AIA_0WGHT / 1000000000.0, AIA_1WGHT / 1000000000.0, AIA_2WGHT / 1000000000.0, AIA_3WGHT / 1000000000.0},
    {AIB_0WGHT / 1000000000.0, AIB_1WGHT / 1000000000.0, AIB_2WGHT / 1000000000.0, AIB_3WGHT / 1000000000.0},
    {AIC_0WGHT / 1000000000.0, AIC_1WGHT / 1000000000.0}
});

// AI offsets
static const std::vector<std::vector<double>> OFFSETS({
    {AIA_0OFST / 1000000000.0, AIA_1OFST / 1000000000.0, AIA_2OFST / 1000000000.0, AIA_3OFST / 1000000000.0},
    {AIB_0OFST / 1000000000.0, AIB_1OFST / 1000000000.0, AIB_2OFST / 1000000000.0, AIB_3OFST / 1000000000.0},
    {AIC_0OFST / 1000000000.0, AIC_1OFST / 1000000000.0}
});

MyRioAI::MyRioAI(MyRio& daq, MyRioConnectorType type, const std::vector<uint32>& channel_numbers) :
  daq_(daq),
  type_(type)
{
    set_name(daq.get_name() + "_AI");
    set_channel_numbers(channel_numbers);
}

bool MyRioAI::update_channel(uint32 channel_number) {
    if (!daq_.is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    NiFpga_Status status;
    uint16_t value = 0;
    status = NiFpga_ReadU16(myrio_session, REGISTERS[type_][channel_number], &value);
    if (status < 0) {
        LOG(Error) << "Failed to update " << get_name() << " channel number "  << channel_number;
        return false;
    }
    else {
        if (type_ == MyRioConnectorType::MspC)
            values_[channel_number] = (int16_t)value * WEIGHTS[type_][channel_number] + OFFSETS[type_][channel_number];
        else
            values_[channel_number] = value * WEIGHTS[type_][channel_number] + OFFSETS[type_][channel_number];
        return true;
    }
}

}  // namespace mel
