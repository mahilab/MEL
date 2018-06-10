#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Daq/NI/MyRio/MyRioAnalogInput.hpp>
#include <MEL/Logging/Log.hpp>

#include "Detail/MyRio.h"

#if !defined(UINT16_MAX)
#define UINT16_MAX 65535
#endif

#if !defined(INT16_MAX)
#define INT16_MAX 0x7fff
#endif

#if !defined(INT16_MIN)
#define INT16_MIN (-INT16_MAX - 1)
#endif

extern NiFpga_Session myrio_session;

namespace mel {

MyRioAnalogInput::MyRioAnalogInput(MyRio& daq)
    : Input(daq.get_name() + "_analog_input", {0, 1}), daq_(daq) {}

MyRioAnalogInput::~MyRioAnalogInput() {

}

bool MyRioAnalogInput::enable() {
    return Device::enable();
}

bool MyRioAnalogInput::disable() {
    return Device::disable();
}

bool MyRioAnalogInput::update() {
    if (update_channel(0) &&
        update_channel(1))
        return true;
    else
        return false;
}

bool MyRioAnalogInput::update_channel(uint32 channel_number) {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }

    NiFpga_Status status;
    uint16_t value = 0;

    uint32_t val;  /**< AIO value register */
    uint32_t wght; /**< AIO weight constant value */
    uint32_t ofst; /**< AIO offset constant value */

    switch (channel_number) {
        case 0:
            val  = AIC_0VAL;
            wght = AIC_0WGHT;
            ofst = AIC_0OFST;
            break;
        case 1:
            val  = AIC_1VAL;
            wght = AIC_1WGHT;
            ofst = AIC_1OFST;
            break;
        default:
            LOG(Error) << "Channel number " << channel_number << "is invalid";
            return false;
    }

    status = NiFpga_ReadU16(myrio_session, val, &value);

    if (status < 0) {
        LOG(Error) << "Failed to update " << get_name() << " channel number "
                   << channel_number;
        return false;
    } else {
        double scale_weight = wght / 1000000000.0;
        double scale_offset = ofst / 1000000000.0;
        values_[channel_map_.at(channel_number)] =
            value * scale_weight + scale_offset;
        return true;
    }
}

}  // namespace mel
