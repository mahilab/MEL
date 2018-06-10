#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include "Detail/MyRio.h"
#include <MEL/Logging/Log.hpp>

namespace mel {

MyRio::MyRio(const std::string& name, bool open) :
    Daq(name),
    analog_input_C(*this)
 {
    if (open) {
        this->open();
    }
}

MyRio::~MyRio() {
    this->close();
}

bool MyRio::open() {
    NiFpga_Status status = MyRio_Open();
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to open myRIO " << get_name();
        return false;
    }
    LOG(Verbose) << "Opened myRIO " << get_name();
    return Daq::open();
}

bool MyRio::close() {
    NiFpga_Status status = MyRio_Close();
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to close myRIO " << get_name();
        return false;
    }
    LOG(Verbose) << "Closed myRIO " << get_name();
    return Daq::close();
}

bool MyRio::enable() {
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    // enable each module
    if (!analog_input_C.enable())
        return false;
    // if (!analog_output.enable())
    //     return false;
    // if (!digital_input.enable())
    //     return false;
    // if (!digital_output.enable())
    //     return false;
    // if (!encoder.enable())
    //     return false;
    // if (!velocity.enable())
    //     return false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::enable();
}

bool MyRio::disable() {
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    // disable each module
    if (!analog_input_C.disable())
        return false;
    // if (!analog_output.disable())
    //     return false;
    // if (!digital_input.disable())
    //     return false;
    // if (!digital_output.disable())
    //     return false;
    // if (!encoder.disable())
    //     return false;
    // if (!velocity.disable())
        return false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::disable();
}

bool MyRio::update_input() {
    if (!analog_input_C.update())
        return false;
    return true;
}

bool MyRio::update_output() {
    return true;
}


}  // namespace mel
