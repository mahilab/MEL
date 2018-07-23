#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include "Detail/MyRio.h"
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// CONNECTOR
//==============================================================================

MyRio::Connector::Connector(MyRio& myrio,
    MyRioConnectorType type,
    const std::vector<uint32>& ai_channels,
    const std::vector<uint32>& ao_channels) :
    Device("myrio_connector_" + std::to_string(type)),
    AI(myrio, type, ai_channels),
    AO(myrio, type, ao_channels)
{}

bool MyRio::Connector::enable() {
    if (AI.enable() && AO.enable())
        return Device::enable();
    else
        return false;
}

bool MyRio::Connector::disable() {
    if (AI.disable() && AO.disable())
        return Device::disable();
    else
        return false;
}

bool MyRio::Connector::update_input() {
    return AI.update();
}

bool MyRio::Connector::update_output() {
    return AO.update();
}

//==============================================================================
// MYRIO
//==============================================================================

MyRio::MyRio(const std::string& name, bool auto_open) :
    Daq(name),
    A(*this, MyRioConnectorType::MxpA, {0,1,2,3}, {0,1}),
    B(*this, MyRioConnectorType::MxpB, {0,1,2,3}, {0,1}),
    C(*this, MyRioConnectorType::MspC, {0,1},     {0,1})
 {
    if (auto_open)
        open();
}

MyRio::~MyRio() {
    if (is_enabled())
        disable();
    if (is_open())
        close();
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
    if (!is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    // enable each connector
    if (A.enable() && B.enable() && C.enable()) {
        sleep(milliseconds(10));
        return Device::enable();
    }
    else
        return false;
}

bool MyRio::disable() {
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    // disable each connect
    if (A.disable() && B.disable() && C.disable()) {
        sleep(milliseconds(10));
        return Device::disable();
    }
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::disable();
}

bool MyRio::update_input() {
    return (A.update_input() && B.update_input() && C.update_input());
}

bool MyRio::update_output() {
    return (A.update_output() && B.update_output() && C.update_output());
}

}  // namespace mel
