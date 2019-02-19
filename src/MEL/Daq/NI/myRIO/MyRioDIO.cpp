#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>
#include "Detail/MyRioFpga60/MyRio.h"

extern NiFpga_Session myrio_session;

namespace mel {

namespace {

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

} // namespace

MyRioDIO::MyRioDIO(MyRioConnector& connector, const ChanNums& channel_numbers) :
    DigitalInputOutput(channel_numbers),
    connector_(connector)
{
    set_name(connector_.get_name() + "_DIO");
}

bool MyRioDIO::update_channel(ChanNum channel_number) {
    NiFpga_Status status;
    uint8_t dirValue;
    if (directions_[channel_number] == In) {
        // read value
        uint8_t inValue;
        uint8_t dirMask;
        status = NiFpga_ReadU8(myrio_session, DIRS[connector_.type][channel_number], &dirValue);
        if (status < 0) {
            LOG(Error) << "Could not read from DIO channel direction register";
            return false;
        }
        dirMask = 1 << BITS[connector_.type][channel_number];
        dirMask = ~dirMask;
        dirValue = dirValue & dirMask;
        status = NiFpga_WriteU8(myrio_session, DIRS[connector_.type][channel_number], dirValue);
        NiFpga_MergeStatus(&status, NiFpga_ReadU8(myrio_session, INS[connector_.type][channel_number], &inValue));
        if (status < 0) {
            LOG(Error) << "Could not write to/read from the DIO channel direction/in register";
            return false;
        }
        inValue = inValue & (1 << BITS[connector_.type][channel_number]);
        values_[channel_number] = (inValue > 0) ? High : Low;
    }
    else {
        // write value
        uint8_t outValue;
        NiFpga_Bool value = static_cast<NiFpga_Bool>(values_[channel_number]);
        status = NiFpga_ReadU8(myrio_session, OUTS[connector_.type][channel_number], &outValue);
        NiFpga_MergeStatus(&status,  NiFpga_ReadU8(myrio_session, DIRS[connector_.type][channel_number], &dirValue));
        if (status < 0) {
            LOG(Error) << "Could not read from the DIO channel registers!";
            return false;
        }
        dirValue = dirValue | (1 << BITS[connector_.type][channel_number]);
        outValue = outValue & ~(1 << BITS[connector_.type][channel_number]);
        outValue = outValue | (value << BITS[connector_.type][channel_number]);
        NiFpga_MergeStatus(&status, NiFpga_WriteU8(myrio_session, OUTS[connector_.type][channel_number], outValue));
        NiFpga_MergeStatus(&status, NiFpga_WriteU8(myrio_session, DIRS[connector_.type][channel_number], dirValue));
        if (status < 0) {
            LOG(Error) << "Could not write to the DIO chanel registers!";
            return false;
        }
    }
    return true;
}

}  // namespace mel
