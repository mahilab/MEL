#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>
#include "Detail/MyRioFpga60/MyRio.h"
#include "Detail/MyRioUtil.hpp"

extern NiFpga_Session myrio_session;

namespace mel {

MyRioDIO::MyRioDIO(MyRioConnector& connector, const ChanNums& channel_numbers) :
    DigitalInputOutput(channel_numbers),
    connector_(connector)
{
    set_name(connector_.get_name() + "_DIO");
    // get registers
    if (connector_.type == MyRioConnector::MxpA) {
        sysselect_ = SYSSELECTA;
        dirs_      = {DIOA_70DIR, DIOA_158DIR};
        ins_       = {DIOA_70IN,  DIOA_158IN};
        outs_      = {DIOA_70OUT, DIOA_158OUT};
    }
    else if (connector_.type == MyRioConnector::MxpB) {
        sysselect_ = SYSSELECTB;
        dirs_      = {DIOB_70DIR, DIOB_158DIR};
        ins_       = {DIOB_70IN,  DIOB_158IN};
        outs_      = {DIOB_70OUT, DIOB_158OUT};
    }
    else if (connector_.type ==MyRioConnector:: MspC) {
        sysselect_ = SYSSELECTC;
        dirs_      = {DIOC_70DIR};
        ins_       = {DIOC_70IN};
        outs_      = {DIOC_70OUT};
    }
}

bool MyRioDIO::update_channel(ChanNum channel_number) {
    if (!validate_channel_number(channel_number))
        return false;
    if (directions_[channel_number] == In) {
        values_[channel_number] = get_register_bit(ins_[channel_number / 8], channel_number % 8) ? High : Low;
    }
    else {
        if (values_[channel_number] == High)
            set_register_bit(outs_[channel_number / 8], channel_number % 8);
        else
            clr_register_bit(outs_[channel_number / 8], channel_number % 8);
    }
    return true;
}

bool MyRioDIO::set_direction(ChanNum channel_number, Direction direction) {
    if (!InputOutput::set_direction(channel_number, direction))
        return false;
    if (direction == Out)
        set_register_bit(dirs_[channel_number / 8], channel_number % 8);
    else
        clr_register_bit(dirs_[channel_number / 8], channel_number % 8);
    return true;
}

void MyRioDIO::sync() {
    // determine which channels are available
    auto bits = read_register(sysselect_);
    ChanNums chs;
    chs.reserve(16);
    if (connector_.type == MyRioConnector::MxpA || connector_.type == MyRioConnector::MxpB) {
        chs = {0,1,2,3,4};
        if (!bits[0]) {
            chs.push_back(5);
            if (!bits[1]) {
                chs.push_back(6);
                chs.push_back(7);
            }
        }
        else if (!bits[1])
            chs.push_back(6);
        if (!bits[2])
            chs.push_back(8);
        if (!bits[3])
            chs.push_back(9);
        if (!bits[4])
            chs.push_back(10);
        if (!bits[5]) {
            chs.push_back(11);
            chs.push_back(12);
        }
        chs.push_back(13);
        if (!bits[7]) {
            chs.push_back(14);
            chs.push_back(15);
        }
    }
    else if (connector_.type == MyRioConnector::MspC) {
        chs = {1, 5};
        if (!bits[0]) {
            chs.push_back(0);
            chs.push_back(2);
        }
        if (!bits[1])
            chs.push_back(3);
        if (!bits[2]) {
            chs.push_back(4);
            chs.push_back(6);
        }
        if (!bits[3])
            chs.push_back(7);
    }
    // set module channels
    set_channel_numbers(chs);
    // update directions
    chs = get_channel_numbers();
    for (auto& ch : chs) {
        Direction dir = get_register_bit(dirs_[ch / 8], ch % 8) ? Out : In;
        set_direction(ch, dir);
    }
}

}  // namespace mel
