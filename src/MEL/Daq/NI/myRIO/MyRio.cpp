#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include "Detail/MyRioFpga60/MyRio.h"
#include <MEL/Logging/Log.hpp>
#include "Detail/MyRioFpga60/MyRio.h"
#include "Detail/MyRioUtil.hpp"
#include <MEL/Core/Clock.hpp>

#define MyRio_DefaultFolder "/var/local/natinst/bitfiles/"
#define MyRio_BitfilePath MyRio_DefaultFolder MyRio_Bitfile

extern NiFpga_Session myrio_session;

namespace mel {

namespace {

/// Custom open function adapted from NI version
bool open_myrio(bool reset) {
    Time timeout = seconds(5);
    NiFpga_Status status;
    NiFpga_Bool sysReady;
    // init
    status = NiFpga_Initialize();
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to initializae myRIO FPGA " << get_nifpga_error_message(status);
        return false;
    }
    // open but don't run yet
    status = NiFpga_Open(MyRio_BitfilePath, MyRio_Signature, "RIO0", NiFpga_OpenAttribute_NoRun, &myrio_session);
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to open myRIO FPGA " << get_nifpga_error_message(status);
        if (status == NiFpga_Status_BitfileReadError) {
            LOG(Error) << "Ensure NiFpga_MyRio1900Fpga60.lvbitx is in " <<  MyRio_DefaultFolder;
        }
        return false;
    }
    // reset FPGA if requested
    if (reset) {
        status = NiFpga_Reset(myrio_session);
        if (MyRio_IsNotSuccess(status)) {
            LOG(Error) << "Failed to reset myRIO FGPA " << get_nifpga_error_message(status);
            return false;
        }
        LOG(Info) << "Reset myRIO FPGA";
    }
    // run FPGA
    status = NiFpga_Run(myrio_session, 0);
    if (MyRio_IsNotSuccess(status)) {
        if (status == -NiFpga_Status_FpgaAlreadyRunning) {
            LOG(Warning) << "myRIO FPGA is already running";
            return true;
        }
        else {
            LOG(Error) << "Failed to run myRIO FPGA " << get_nifpga_error_message(status);
            return false;
        }
    }
    // wait for the FPGA to signal ready
    Clock clock;
    sysReady = NiFpga_False;    
    while (clock.get_elapsed_time() < timeout && !MyRio_IsNotSuccess(status) && !sysReady) {
        NiFpga_MergeStatus(&status, NiFpga_ReadBool(myrio_session, SYSRDY, &sysReady));
    }
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to read SYSRDY register " << get_nifpga_error_message(status);
        return false;
    }
    if (!sysReady) {
        LOG(Error) << "Timeout while waiting for system ready";
        return false;
    }
    return true;
}

/// Custom close function adapted from NI version
bool close_myrio(bool reset) {
    NiFpga_Status status;
    // close FPGA
    status = reset ? NiFpga_Close(myrio_session, 0) : NiFpga_Close(myrio_session, NiFpga_CloseAttribute_NoResetIfLastSession);
    if (MyRio_IsNotSuccess(status)) {
        LOG(Error) << "Failed to close myRIO FPGA " << get_nifpga_error_message(status);
        return false;
    }
    // unload the NiFpga library
    status = NiFpga_Finalize();
    if (MyRio_IsNotSuccess(status)) {
        MyRio_PrintStatus(status);
        LOG(Error) << "Failed to unload myRIO FPGA library " << get_nifpga_error_message(status);
        return false;
    }
    return true;
}

}

MyRio::MyRio() :
    DaqBase("myRIO"),
    mxpA(*this, MyRioConnector::Type::MxpA, {0,1,2,3}, {0,1}, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, {}),
    mxpB(*this, MyRioConnector::Type::MxpB, {0,1,2,3}, {0,1}, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, {}),
    mspC(*this, MyRioConnector::Type::MspC, {0,1},     {0,1}, {0,1,2,3,4,5,6,7}, {})
{
}

MyRio::~MyRio() {
    if (is_enabled())
        disable();
    if (is_open())
        close();
}

bool MyRio::on_open() {    
    return (open_myrio(false) &&  mxpA.open() &&  mxpB.open() && mspC.open());
}

bool MyRio::on_close() {
    return (close_myrio(false) && mxpA.close() && mxpB.close() && mspC.close());
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
