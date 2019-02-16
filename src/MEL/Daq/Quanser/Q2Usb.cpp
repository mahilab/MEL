#include <MEL/Daq/Quanser/Q2Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// STATIC VARIABLES
//==============================================================================

 uint32 NEXT_Q2USB_ID = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Q2Usb::Q2Usb(QuanserOptions options, uint32 id) :
    QuanserDaq("q2_usb", id, options),
    AI(*this,     {0,1}),
    AO(*this,     {0,1}),
    DIO(*this,    {0,1,2,3,4,5,6,7,8}),
    encoder(*this,{0,1}),
    watchdog(*this, milliseconds(100))
{
    // increment next_id_
    ++NEXT_Q2USB_ID;
    // disable velocity estimation
    encoder.has_velocity(false);
}


Q2Usb::~Q2Usb() {

    // if enabled, disable
    if (is_enabled())
        disable();
    // if open, close
    if (is_open()) {
        // set default options on program end
        set_options(QuanserOptions());
        close();
    }
    // decrement next_id_
    --NEXT_Q2USB_ID;
}

bool Q2Usb::on_open() {
    // open as QDaq
    if (!QuanserDaq::on_open())
        return false;
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // set DIO directions (all DIO In, LED Out)
    DIO.set_directions({ In,In,In,In,In,In,In,In,Out });
    // set default expire values (digital = LOW, analog = 0.0V)
    if (!AO.set_expire_values(std::vector<Voltage>(2, 0.0))) {
        close();
        return false;
    }
    if (!DIO.set_expire_values(std::vector<Logic>(9, Low))) {
        close();
        return false;
    }
    // allow changes to take effect
    sleep(milliseconds(10));
    return true;
}

bool Q2Usb::on_close() {
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    // close as QDaq
    return QuanserDaq::on_close();
}

bool Q2Usb::on_enable() {
    if (!is_open()) {
        LOG(Error) << "Unable to enable Q2-USB " << get_name() << " because it is not open";
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

bool Q2Usb::on_disable() {
    if (!is_open()) {
        LOG(Error) << "Unable to disable Q2-USB " << get_name() << " because it is not open";
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
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    return success;
}

bool Q2Usb::update_input() {
    if (!is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    if (AI.update() && encoder.update() && DIO.update())
        return true;
    else {
        return false;
    }
}

bool Q2Usb::update_output() {
    if (!is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    if (AO.update() && DIO.update())
        return true;
    else
        return false;
}

bool Q2Usb::identify(uint32 input_channel_number, uint32 outout_channel_number) {
    if (!is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    InputOutput<Logic>::Channel di_ch = DIO.get_channel(input_channel_number);
    InputOutput<Logic>::Channel do_ch = DIO.get_channel(outout_channel_number);
    for (int i = 0; i < 5; ++i) {
        do_ch.set_value(High);
        do_ch.update();
        sleep(milliseconds(10));
        di_ch.update();
        if (di_ch.get_value() != High)
            return false;
        do_ch.set_value(Low);
        do_ch.update();
        sleep(milliseconds(10));
        di_ch.update();
        if (di_ch.get_value() != Low)
            return false;
    }
    return true;
}

void Q2Usb::set_led(Logic value) {
    DIO[8].set_value(value);
}

std::size_t Q2Usb::get_q2_usb_count() {
    return QuanserDaq::get_qdaq_count("q2_usb");
}

uint32 Q2Usb::next_id() {
    return NEXT_Q2USB_ID;
}

} // namespace mel

