#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRioUtil.hpp"
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>

extern NiFpga_Session myrio_session;

namespace mel {

namespace {

// status registers
static const std::vector<std::vector<uint32_t>> STAT ({
    {ENCASTAT},
    {ENCBSTAT},
    {ENCC_0STAT, ENCC_1STAT}
});

// count value registers
static const std::vector<std::vector<uint32_t>> CNTR ({
    {ENCACNTR},
    {ENCBCNTR},
    {ENCC_0CNTR, ENCC_1CNTR}
});


} // namespace

MyRioEncoder::MyRioEncoder(MyRioConnector& connector, const std::vector<uint32>& channel_numbers) :
    Encoder(channel_numbers),
    connector_(connector)
{
    set_name(connector_.get_name() + "_encoder");
}

bool MyRioEncoder::update_channel(uint32 channel_number) {
    NiFpga_Status status;
    uint32_t counterValue;
    status = NiFpga_ReadU32(myrio_session, CNTR[connector_.type][channel_number], &counterValue);
    if (status < 0) {
        LOG(Error) << "Could not read from the encoder counter register!";
        return false;
    }
    values_[channel_number] = static_cast<int32>(counterValue);
    return true;
}

} // namespace mel
