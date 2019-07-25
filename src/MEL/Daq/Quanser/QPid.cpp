#include <MEL/Daq/Quanser/QPid.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// STATIC VARIABLES
//==============================================================================

 uint32 NEXT_QPID_ID = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QPid::QPid(QuanserOptions options,
             uint32 id) :
    QuanserDaq("qpid_e", id, options),
    AI(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    AO(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    DIO(*this, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
                 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                 50, 51, 52, 53, 54, 55  }),
    encoder(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }, true), // has velocity estimation
    watchdog(*this, milliseconds(100))
{
    // increment NEXT_ID
    ++NEXT_QPID_ID;
}

QPid::~QPid() {
    // if enabled, disable
    if (is_enabled())
        disable();
    // if open, close
    if (is_open()) {
        // set default options on program end
        set_options(QuanserOptions());
        close();
    }
    // decrement NEXT_ID
    --NEXT_QPID_ID;
}

bool QPid::on_open() {
    // open as QDaq
    if (!QuanserDaq::on_open())
        return false;
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // set default expire values (digital = LOW, analog = 0.0V)
    if (!AO.set_expire_values(std::vector<Voltage>(8, 0.0))) {
        close();
        return false;
    }
    if (!DIO.set_expire_values(std::vector<Logic>(56, Low))) {
        close();
        return false;
    }
    // allow changes to take effect
    sleep(milliseconds(10));
    return true;
}

bool QPid::on_close() {
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    // close as QDaq
    return QuanserDaq::on_close();
}

bool QPid::on_enable() {
    if (!is_open()) {
        LOG(Error) << "Unable to enable Q8-USB " << get_name() << " because it is not open";
        return false;
    }
    bool success = true;
    // enable each module
    if (!AI.enable())
        success = false;
    if (!AO.enable())
        success = false;
    if (!DIO.enable())
        success = false;
    if (!encoder.enable())
        success = false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return success;
}

bool QPid::on_disable() {
    if (!is_open()) {
        LOG(Error) << "Unable to disable Q8-USB " << get_name() << " because it is not open";
        return false;
    }
    bool success = true;
    // disable each module
    if (!AI.disable())
        success = false;
    if (!AO.disable())
        success = false;
    if (!DIO.disable())
        success = false;
    if (!encoder.disable())
        success = false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return success;
}

bool QPid::update_input() {
    // TODO: Use hil_read like Q8 USB
    if (!is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    if (AI.update() && encoder.update() && DIO.update_input())
        return true;
    else {
        return false;
    }
}

bool QPid::update_output() {
    // TODO: Use hil_write like Q8 USB
    if (!is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    if (AO.update() && DIO.update_output())
        return true;
    else
        return false;
}

uint32 QPid::next_id() {
    return NEXT_QPID_ID;
}

} // namespace mel

