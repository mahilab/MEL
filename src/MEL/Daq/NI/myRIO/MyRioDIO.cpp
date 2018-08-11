#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRio.h"

extern NiFpga_Session myrio_session;

namespace mel {

// direction registers
static const std::vector<std::vector<uint32_t>> DIRS({
    {DIOA_70DIR, DIOA_70DIR, DIOA_70DIR, DIOA_70DIR, DIOA_70DIR, DIOA_70DIR, DIOA_70DIR, DIOA_70DIR,
        DIOA_158DIR, DIOA_158DIR, DIOA_158DIR, DIOA_158DIR, DIOA_158DIR, DIOA_158DIR, DIOA_158DIR, DIOA_158DIR},
    {DIOB_70DIR, DIOB_70DIR, DIOB_70DIR, DIOB_70DIR, DIOB_70DIR, DIOB_70DIR, DIOB_70DIR, DIOB_70DIR,
        DIOB_158DIR, DIOB_158DIR, DIOB_158DIR, DIOB_158DIR, DIOB_158DIR, DIOB_158DIR, DIOB_158DIR, DIOB_158DIR},
    {DIOC_70DIR, DIOC_70DIR, DIOC_70DIR, DIOC_70DIR, DIOC_70DIR, DIOC_70DIR, DIOC_70DIR, DIOC_70DIR}
});

// out value registers
static const std::vector<std::vector<uint32_t>> INS({
    {DIOA_70IN, DIOA_70IN, DIOA_70IN, DIOA_70IN, DIOA_70IN, DIOA_70IN, DIOA_70IN, DIOA_70IN,
        DIOA_158IN, DIOA_158IN, DIOA_158IN, DIOA_158IN, DIOA_158IN, DIOA_158IN, DIOA_158IN, DIOA_158IN},
    {DIOB_70IN, DIOB_70IN, DIOB_70IN, DIOB_70IN, DIOB_70IN, DIOB_70IN, DIOB_70IN, DIOB_70IN,
        DIOB_158IN, DIOB_158IN, DIOB_158IN, DIOB_158IN, DIOB_158IN, DIOB_158IN, DIOB_158IN, DIOB_158IN},
    {DIOC_70IN, DIOC_70IN, DIOC_70IN, DIOC_70IN, DIOC_70IN, DIOC_70IN, DIOC_70IN, DIOC_70IN}
});

// in value registers
static const std::vector<std::vector<uint32_t>> OUTS({
    {DIOA_70OUT, DIOA_70OUT, DIOA_70OUT, DIOA_70OUT, DIOA_70OUT, DIOA_70OUT, DIOA_70OUT, DIOA_70OUT,
        DIOA_158OUT, DIOA_158OUT, DIOA_158OUT, DIOA_158OUT, DIOA_158OUT, DIOA_158OUT, DIOA_158OUT, DIOA_158OUT},
    {DIOB_70OUT, DIOB_70OUT, DIOB_70OUT, DIOB_70OUT, DIOB_70OUT, DIOB_70OUT, DIOB_70OUT, DIOB_70OUT,
        DIOB_158OUT, DIOB_158OUT, DIOB_158OUT, DIOB_158OUT, DIOB_158OUT, DIOB_158OUT, DIOB_158OUT, DIOB_158OUT},
    {DIOC_70OUT, DIOC_70OUT, DIOC_70OUT, DIOC_70OUT, DIOC_70OUT, DIOC_70OUT, DIOC_70OUT, DIOC_70OUT}
});

// in value registers
static const std::vector<std::vector<uint8_t>> BITS({
    {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7},
    {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7},
    {0, 1, 2, 3, 4, 5, 6, 7}
});

MyRioDIO::MyRioDIO(MyRio& daq, MyRioConnectorType type, const std::vector<uint32>& channel_numbers) :
    daq_(daq),
    type_(type)
{
    set_name(daq.get_name() + "_DIO");
    set_channel_numbers(channel_numbers);
}

bool MyRioDIO::on_enable() {
    set_values(enable_values_.get());
    if (update()) {
        LOG(Verbose) << "Set " << get_name() << " enable values to "
                     << enable_values_;
        return true;
    } else {
        LOG(Error) << "Failed to set " << get_name() << " enable values to "
                   << enable_values_;
        return false;
    }
}

bool MyRioDIO::on_disable() {
    set_values(disable_values_.get());
    if (update()) {
        LOG(Verbose) << "Set " << get_name() << " disable values to "
                     << disable_values_;
        return true;
    } else {
        LOG(Error) << "Failed to set " << get_name() << " disable values to "
                   << disable_values_;
        return false;
    }
}

bool MyRioDIO::update_channel(uint32 channel_number) {
    NiFpga_Status status;
    uint8_t dirValue;
    if (directions_[channel_number] == In) {
        // read value
        uint8_t inValue;
        uint8_t dirMask;
        status = NiFpga_ReadU8(myrio_session, DIRS[type_][channel_number], &dirValue);
        if (status < 0) {
            LOG(Error) << "Could not read from DIO channel direction register";
            return false;
        }
        dirMask = 1 << BITS[type_][channel_number];
        dirMask = ~dirMask;
        dirValue = dirValue & dirMask;
        status = NiFpga_WriteU8(myrio_session, DIRS[type_][channel_number], dirValue);
        NiFpga_MergeStatus(&status, NiFpga_ReadU8(myrio_session, INS[type_][channel_number], &inValue));
        if (status < 0) {
            LOG(Error) << "Could not write to/read from the DIO channel direction/in register";
            return false;
        }
        inValue = inValue & (1 << BITS[type_][channel_number]);
        values_[channel_number] = (inValue > 0) ? High : Low;
    }
    else {
        // write value
        uint8_t outValue;
        NiFpga_Bool value = static_cast<NiFpga_Bool>(values_[channel_number]);
        status = NiFpga_ReadU8(myrio_session, OUTS[type_][channel_number], &outValue);
        NiFpga_MergeStatus(&status,  NiFpga_ReadU8(myrio_session, DIRS[type_][channel_number], &dirValue));
        if (status < 0) {
            LOG(Error) << "Could not read from the DIO channel registers!";
            return false;
        }
        dirValue = dirValue | (1 << BITS[type_][channel_number]);
        outValue = outValue & ~(1 << BITS[type_][channel_number]);
        outValue = outValue | (value << BITS[type_][channel_number]);
        NiFpga_MergeStatus(&status, NiFpga_WriteU8(myrio_session, OUTS[type_][channel_number], outValue));
        NiFpga_MergeStatus(&status, NiFpga_WriteU8(myrio_session, DIRS[type_][channel_number], dirValue));
        if (status < 0) {
            LOG(Error) << "Could not write to the DIO chanel registers!";
            return false;
        }
    }
    return true;
}

}  // namespace mel
