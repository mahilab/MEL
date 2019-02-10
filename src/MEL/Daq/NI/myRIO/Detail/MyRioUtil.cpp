#include "MyRioUtil.hpp"
#include <MEL/Logging/Log.hpp>
#include "MyRioFpga60/MyRio.h"
#include <cassert>

extern NiFpga_Session myrio_session;

namespace mel {

void set_register_bit(unsigned int reg, int i) {
    assert(i >= 0 && i < 8);
    uint8_t bits;
    NiFpga_Status status;
    status = NiFpga_ReadU8(myrio_session, reg, &bits);
    if (status < 0) {
        LOG(Error) << "Could not read from system select register";
        return;
    }
    bits |= (1UL << i);
    status = NiFpga_WriteU8(myrio_session, reg, bits);
    if (status < 0) {
        LOG(Error) << "Could not write to the system select register";
    }
}

void clr_register_bit(unsigned int reg, int i) {
    assert(i >= 0 && i < 8);
    uint8_t bits;
    NiFpga_Status status;
    status = NiFpga_ReadU8(myrio_session, reg, &bits);
    if (status < 0) {
        LOG(Error) << "Could not read from system select register";
        return;
    }
    bits &= ~(1UL << i);
    status = NiFpga_WriteU8(myrio_session, reg, bits);
    if (status < 0) {
        LOG(Error) << "Could not write to the system select register";
    }
}

bool get_register_bit(unsigned int reg, int i) {
    assert(i >= 0 && i < 8);
    uint8_t bits;
    NiFpga_Status status;
    status = NiFpga_ReadU8(myrio_session, reg, &bits);
    if (status < 0) {
        LOG(Error) << "Could not read from system select register";
    }
    return ((bits >> i) & 1) != 0;
}



} // namespace mel
