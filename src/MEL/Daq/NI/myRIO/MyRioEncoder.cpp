#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRioUtil.hpp"
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>
#include <algorithm>

extern NiFpga_Session myrio_session;

namespace mel {

MyRioEncoder::MyRioEncoder(MyRioConnector& connector, const ChanNums& channel_numbers) :
    Encoder(channel_numbers),
    connector_(connector)
{
    set_name(connector_.get_name() + "_encoder");
    // get registers
    if (connector_.type == MyRioConnector::MxpA) {
        sysselect_ = SYSSELECTA;
        bits_      = {5};
        stat_      = {ENCASTAT};
        cntr_      = {ENCACNTR};
        cnfg_      = {ENCACNFG};
        allowed_   = {0};
    }
    else if (connector_.type == MyRioConnector::MxpB) {
        sysselect_ = SYSSELECTB;
        bits_      = {5};
        stat_      = {ENCBSTAT};
        cntr_      = {ENCBCNTR};
        cnfg_      = {ENCBCNFG};
        allowed_   = {0};
    }
    else if (connector_.type == MyRioConnector:: MspC) {
        sysselect_ = SYSSELECTC;
        bits_      = {0,2};
        stat_      = {ENCC_0STAT, ENCC_1STAT};
        cntr_      = {ENCC_0CNTR, ENCC_1CNTR};
        cnfg_      = {ENCC_0CNFG, ENCC_1CNFG};
        allowed_   = {0,1};
    }
}

bool MyRioEncoder::update_channel(ChanNum channel_number) {
    if (!validate_channel_number(channel_number))
        return false;
    uint32_t counts;
    NiFpga_Status status = NiFpga_ReadU32(myrio_session, cntr_[channel_number], &counts);
    if (status < 0) {
        LOG(Error) << "Failed to read counts from encoder register";
        return false;
    }
    values_[channel_number] = static_cast<int32>(counts);
    return true;
}

bool MyRioEncoder::reset_count(ChanNum channel_number, int count) {
    if (!validate_channel_number(channel_number))
        return false;
    if (count == 0) {
        set_register_bit(cnfg_[channel_number], 1);
        clr_register_bit(cnfg_[channel_number], 1);
        values_[channel_number] = 0;
        return true;
    }
    else  {
        LOG(Error) << "myRIO Encoder counts can only be reset to zero";
        return false;
    }
}

void MyRioEncoder::enable_channel(ChanNum ch) {
    if (std::count(allowed_.begin(), allowed_.end(), ch)) {
        // add to module
        add_channel_number(ch);
        // configure FPGA
        set_register_bit(sysselect_, bits_[ch]); // enable encoder on FPGA
        clr_register_bit(cnfg_[ch], 2);          // set to quadrature mode
        set_register_bit(cnfg_[ch], 0);          // enable encoder
        // sync DIO pins
        connector_.DIO.sync();
    }
    else {
        LOG(Error) << "Encoder channel number " << ch << " not available on " << get_name();
    }
}

void MyRioEncoder::disable_channel(ChanNum ch) {
    if (std::count(allowed_.begin(), allowed_.end(), ch)) {
        // remove off module
        remove_channel_number(ch);
        // configure FPGA
        clr_register_bit(sysselect_, bits_[ch]); // disable encoder on FPGA
        clr_register_bit(cnfg_[ch], 0);          // disable encoder
        // sync DIO pins
        connector_.DIO.sync();
    }
    else {
        LOG(Error) << "Encoder channel number " << ch << " not available on " << get_name();
    }
}

void MyRioEncoder::sync() {
    // determine which channels are currently enabled on FPGA
    ChanNums chs;
    for (auto& ch : allowed_) {
        if (get_register_bit(sysselect_, bits_[ch])) {
            chs.push_back(ch);
            clr_register_bit(cnfg_[ch], 2); // set to quadrature mode
            set_register_bit(cnfg_[ch], 0); // enable encoder
        }
    }
    // set module channels
    set_channel_numbers(chs);
}

} // namespace mel
