#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// STATIC VARIABLES
//==============================================================================

 uint32 NEXT_Q8USB_ID = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Q8Usb::Q8Usb(QuanserOptions options,
             bool perform_sanity_check,
             uint32 id) :
    QuanserDaq("q8_usb", id, options),
    perform_sanity_check_(perform_sanity_check),
    AI(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    AO(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    DI(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    DO(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    encoder(*this, { 0, 1, 2, 3, 4, 5, 6, 7 }),
    watchdog(*this, milliseconds(100))
{
    // increment NEXT_ID
    ++NEXT_Q8USB_ID;
    // enable velocity estimation
    encoder.has_velocity(true);
}

Q8Usb::~Q8Usb() {
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
    --NEXT_Q8USB_ID;
}

bool Q8Usb::on_open() {
    // open as QDaq
    if (!QuanserDaq::on_open())
        return false;
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // sanity check
    if (perform_sanity_check_) {
        if (!sanity_check()) {
            on_close();
            return on_open();
        }
    }
    // set default expire values (digital = LOW, analog = 0.0V)
    if (!AO.set_expire_values(std::vector<Voltage>(8, 0.0))) {
        close();
        return false;
    }
    if (!DO.set_expire_values(std::vector<Logic>(8, Low))) {
        close();
        return false;
    }
    // allow changes to take effect
    sleep(milliseconds(10));
    return true;
}

bool Q8Usb::on_close() {
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    // close as QDaq
    return QuanserDaq::on_close();
}

bool Q8Usb::on_enable() {
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
    if (!DI.enable())
        success = false;
    if (!DO.enable())
        success = false;
    if (!encoder.enable())
        success = false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return success;
}

bool Q8Usb::on_disable() {
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
    if (!DI.disable())
        success = false;
    if (!DO.disable())
        success = false;
    if (!encoder.disable())
        success = false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return success;
}

bool Q8Usb::update_input() {
    if (!is_enabled()) {
        LOG(Error) << "Unable to update " << get_name() << " input because it is disabled";
        return false;
    }
    t_error result;
    result = hil_read(handle_,
        AI.get_channel_count() > 0 ? &(AI.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(AI.get_channel_count()),
        encoder.get_channel_count() > 0 ? &(encoder.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(encoder.get_channel_count()),
        DI.get_channel_count() > 0 ? &(DI.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(DI.get_channel_count()),
        encoder.get_channel_count() > 0 ? &(encoder.get_quanser_velocity_channels())[0] : NULL,
        static_cast<uint32>(encoder.get_channel_count()),
        AI.get_channel_count() > 0 ? &(AI.get_values())[0] : NULL,
        encoder.get_channel_count() > 0 ? &(encoder.get_values())[0] : NULL,
        DI.get_channel_count() > 0 ? &(DI.get_quanser_values())[0] : NULL,
        encoder.get_channel_count() > 0 ? &(encoder.get_values_per_sec())[0] : NULL);
    for (std::size_t i = 0; i < DI.get_channel_count(); ++i)
        DI.get_values()[i] = static_cast<Logic>(DI.get_quanser_values()[i]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " input "
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool Q8Usb::update_output() {
    if (!is_enabled()) {
        LOG(Error) << "Unable to update " << get_name() << " output because it is disabled";
        return false;
    }
    // convert digitals
    for (std::size_t i = 0; i < DO.get_channel_count(); ++i)
        DO.get_quanser_values()[i] = static_cast<char>(DO.get_values()[i]);
    t_error result;
    result = hil_write(handle_,
        AO.get_channel_count() > 0 ? &(AO.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(AO.get_channel_count()),
        NULL, 0,
        DO.get_channel_count() > 0 ? &(DO.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(DO.get_channel_count()),
        NULL, 0,
        AO.get_channel_count() > 0 ? &(AO.get_values())[0] : NULL,
        NULL,
        DO.get_channel_count() > 0 ? &(DO.get_quanser_values())[0] : NULL,
        NULL);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " output "
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool Q8Usb::identify(uint32 channel_number) {
    if (!is_open()) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    Input<Logic>::Channel di_ch = DI.get_channel(channel_number);
    Output<Logic>::Channel do_ch = DO.get_channel(channel_number);
    for (int i = 0; i < 5; ++i) {
        do_ch.set_value(High);
        do_ch.update();
        sleep(milliseconds(10));
        di_ch.update();
        if (di_ch.get_value() != High) {
            return false;
        }
        do_ch.set_value(Low);
        do_ch.update();
        sleep(milliseconds(10));
        di_ch.update();
        if (di_ch.get_value() != Low) {
            return false;
        }
    }
    return true;
}

int Q8Usb::identify() {
    for (uint32 channel_number = 0; channel_number < 8; ++channel_number) {
        if (identify(channel_number))
            return (int)channel_number;
    }
    return -1;
}

bool Q8Usb::sanity_check() {
    encoder.update();
    bool sane = true;
    std::vector<double> velocities = encoder.get_values_per_sec();
    for (auto it = velocities.begin(); it != velocities.end(); ++it) {
        if (*it != 0.0) {
            sane = false;
            LOG(Warning) << "Sanity check on " << get_name() << " failed";
            break;
        }
    }
    if (sane)
        LOG(Verbose) << "Sanity check on " << get_name() << " passed";
    return sane;
}

std::size_t Q8Usb::get_q8_usb_count() {
    return QuanserDaq::get_qdaq_count("q8_usb");
}

uint32 Q8Usb::next_id() {
    return NEXT_Q8USB_ID;
}

} // namespace mel

