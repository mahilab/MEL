#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Daq/NI/MyRio/MyRioAI.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>
#include "Detail/MyRioFpga60/MyRio.h"

extern NiFpga_Session myrio_session;

namespace mel {

namespace {

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

} // namespace

MyRioAI::MyRioAI(MyRioConnector& connector, const ChanNums& channel_numbers) :
    AnalogInput(channel_numbers),
    connector_(connector)
{
    set_name(connector_.get_name() + "_AI");
}

bool MyRioAI::update_channel(ChanNum channel_number) {
    if (!connector_.is_open()) {
        LOG(Error) << "Failed to update channel because" << connector_.get_name() << " is not open";
        return false;
    }
    uint16_t value = 0;
    NiFpga_Status status = NiFpga_ReadU16(myrio_session, REGISTERS[connector_.type][channel_number], &value);
    if (status < 0) {
        LOG(Error) << "Failed to update " << get_name() << " channel number "  << channel_number;
        return false;
    }
    else {
        if (connector_.type == MyRioConnector::Type::MspC)
            values_[channel_number] = (int16_t)value * WEIGHTS[connector_.type][channel_number] + OFFSETS[connector_.type][channel_number];
        else
            values_[channel_number] = value * WEIGHTS[connector_.type][channel_number] + OFFSETS[connector_.type][channel_number];
        return true;
    }
}

}  // namespace mel
