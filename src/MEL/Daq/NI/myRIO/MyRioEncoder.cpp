#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRioUtil.hpp"
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>

extern NiFpga_Session myrio_session;

namespace mel {

namespace {

// configure registers
static const std::vector<std::vector<uint32_t>> CNFG ({
    {ENCACNFG},
    {ENCBCNFG},
    {ENCC_0CNFG, ENCC_1CNFG}
});

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
    connector_(connector)
{
    set_name(connector_.get_name() + "_encoder");
    set_channel_numbers(channel_numbers);
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

bool MyRioEncoder::on_enable() {

    auto channel_numbers = get_channel_numbers();

    // configure for MXP connector A or B
    if (connector_.type == MyRioConnector::Type::MxpA ||
        connector_.type == MyRioConnector::Type::MxpB)
    {
        if (channel_numbers.size() == 0) {
            // do nothing
        }
        else if (channel_numbers.size() == 1 && channel_numbers[0] == 0) {
            if (connector_.type == MyRioConnector::Type::MxpA)
                set_register_bit(SYSSELECTA, 5);
            else
                set_register_bit(SYSSELECTB, 5);
        }
        else {
            LOG(Error) << "myRIO MXP connectors A and B only support one encoder channel (channel 0)";
        }
    }
    // configure for MSP connector
    else if (connector_.type == MyRioConnector::Type::MspC) {
        if (channel_numbers.size() == 0) {
            // do nothing
        }
        else if (channel_numbers.size() == 1) {
            if (channel_numbers[0] == 0)
                set_register_bit(SYSSELECTC, 0);
            else if (channel_numbers[0] == 1)
                set_register_bit(SYSSELECTC, 2);
            else
                LOG(Error) << "myRIO MSP connector C only supports channel numbers 0 and 1";
        }
        else if (channel_numbers.size() == 2) {
            set_register_bit(SYSSELECTC, 0);
            set_register_bit(SYSSELECTC, 2);
        }
        else {
            LOG(Error) << "myRIO MSP connector C only supports one or two encoder channels";
        }
    }


    // configure
    for (auto& c : channel_numbers) {
        clr_register_bit(CNFG[connector_.type][c],2); // set to quadrature mode
        set_register_bit(CNFG[connector_.type][c],0); // enable encoder
    }


    return Encoder::on_enable();
}

} // namespace mel
