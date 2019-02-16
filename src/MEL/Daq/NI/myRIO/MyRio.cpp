#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRioFpga60/MyRio.h"
#include "Detail/MyRioUtil.hpp"

namespace mel {

MyRio::MyRio(const std::string& name) :
    DaqBase(name),
    mxpA(*this, MyRioConnector::Type::MxpA, {0,1,2,3}, {0,1}, {0,1,2,3,4,5,6,7,8,9,10,13,14,15}, {0}),
    mxpB(*this, MyRioConnector::Type::MxpB, {0,1,2,3}, {0,1}, {0,1,2,3,4,5,6,7,8,9,10,13,14,15}, {0}),
    mspC(*this, MyRioConnector::Type::MspC, {0,1},     {0,1}, {1,3,5,7}, {0,1})
{
}

MyRio::~MyRio() {
    if (is_enabled())
        disable();
    if (is_open())
        close();
}

bool MyRio::on_open() {
    NiFpga_Status status = MyRio_Open();
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to open myRIO " << get_name();
        return false;
    }
    mxpA.open();
    mxpB.open();
    mspC.open();
    return true;
}

bool MyRio::on_close() {
    NiFpga_Status status = MyRio_Close();
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to close myRIO " << get_name();
        return false;
    }
    mxpA.close();
    mxpB.close();
    mspC.close();
    return true;
}

bool MyRio::on_enable() {
    if (!is_open()) {
        LOG(Error) << "Unable to enable myRIO " << get_name() << " because it is not open";
        return false;
    }
    // enable each connector
    if (mxpA.enable() && mxpB.enable() && mspC.enable()) {
        sleep(milliseconds(10));
        return true;
    }
    else
        return false;
}

bool MyRio::on_disable() {
    if (!is_open()) {
        LOG(Error) << "Unable to disable myRIO " << get_name() << " because it is not open";
        return false;
    }
    // disable each connect
    if (mxpA.disable() && mxpB.disable() && mspC.disable()) {
        sleep(milliseconds(10));
        return true;
    }
    else
        return false;
}

bool MyRio::update_input() {
    return (mxpA.update_input() && mxpB.update_input() && mspC.update_input());
}

bool MyRio::update_output() {
    return (mxpA.update_output() && mxpB.update_output() && mspC.update_output());
}

bool MyRio::is_button_pressed() const {
    return get_register_bit(DIBTN, 0);
}

void MyRio::set_led(int led, bool on) {
    if (on)
        set_register_bit(DOLED30, led);
    else
        clr_register_bit(DOLED30, led);
}

}  // namespace mel
