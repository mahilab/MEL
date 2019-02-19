#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRioUtil.hpp"
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>

extern NiFpga_Session myrio_session;

namespace mel {

namespace {

// system select registers
static const std::vector<uint32_t> SYSSELECT({SYSSELECTA, SYSSELECTB, SYSSELECTC});

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

// encoder configure registers
static const std::vector<std::vector<uint32_t>> CNFG ({
    {ENCACNFG},
    {ENCBCNFG},
    {ENCC_0CNFG, ENCC_1CNFG}
});


} // namespace

MyRioEncoder::MyRioEncoder(MyRioConnector& connector, const ChanNums& channel_numbers) :
    Encoder(channel_numbers),
    connector_(connector)
{
    set_name(connector_.get_name() + "_encoder");
}

bool MyRioEncoder::update_channel(ChanNum channel_number) {
    uint32_t counterValue;
    NiFpga_Status status = NiFpga_ReadU32(myrio_session, CNTR[connector_.type][channel_number], &counterValue);
    if (status < 0) {
        LOG(Error) << "Could not read from the encoder counter register!";
        return false;
    }
    values_[channel_number] = static_cast<int32>(counterValue);
    return true;
}

bool MyRioEncoder::reset_count(ChanNum channel_number, int count) {
    LOG(Error) << "myRIO Encoders do not support resetting counts";
    return false;
}


void MyRioEncoder::sync_from_myrio() {
    // determine how many encoders are enabled and reconfigure
    if (connector_.type == MyRioConnector::MxpA || connector_.type == MyRioConnector::MxpB) {
        if (get_register_bit(SYSSELECT[connector_.type], 5))
            set_channel_numbers({0});
    }
    else if (connector_.type == MyRioConnector::MspC) {
        std::vector<uint32> channel_numbers;
        if (get_register_bit(SYSSELECT[connector_.type], 0))
            channel_numbers.push_back(0);
        if (get_register_bit(SYSSELECT[connector_.type], 2))
            channel_numbers.push_back(0);      
        set_channel_numbers(channel_numbers);
    }
}

void MyRioEncoder::sync_to_myrio() {
    // MXP Connectors
    if (connector_.type == MyRioConnector::MxpA || connector_.type == MyRioConnector::MxpB) {
        // channel 0
        if (validate_channel_number(0, true))
            set_register_bit(SYSSELECT[connector_.type], 5);
        else
            clr_register_bit(SYSSELECT[connector_.type], 5);
    }
    // MSP Connector
    else if (connector_.type == MyRioConnector::MspC) {
        // channel 0
        if (validate_channel_number(0, true))
            set_register_bit(SYSSELECT[connector_.type], 0);
        else
            clr_register_bit(SYSSELECT[connector_.type], 0);
        // channel 1
        if (validate_channel_number(1, true))
            set_register_bit(SYSSELECT[connector_.type], 2);
        else
            clr_register_bit(SYSSELECT[connector_.type], 2);
    }
    LOG(Verbose) << "Configured myRIO Connector " << get_name() << " for Encoders channels [" << get_channel_numbers() << "]"; 
}

bool MyRioEncoder::on_enable() {
    for (auto &c : get_channel_numbers()) {
        clr_register_bit(CNFG[connector_.type][c], 2); // set to quadrature mode
        set_register_bit(CNFG[connector_.type][c], 0); // enable encoder
    }
    return true;
}


} // namespace mel
