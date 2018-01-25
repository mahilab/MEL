#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// STATIC MEMBER VARIABLES
//==============================================================================

 uint32 Q8Usb::next_id_ = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================


Q8Usb::Q8Usb(QOptions options,
             bool open,
             bool perform_sanity_check,
             const std::vector<uint32>& ai_channels,
             const std::vector<uint32>& ao_channels,
             const std::vector<uint32>& di_channels,
             const std::vector<uint32>& do_channels,
             const std::vector<uint32>& enc_channels,
             uint32 id) :
    QDaq("q8_usb", id, options),
    perform_sanity_check_(perform_sanity_check),
    analog_input(*this, ai_channels),
    analog_output(*this, ao_channels),
    digital_input(*this, di_channels),
    digital_output(*this, do_channels),
    encoder(*this, enc_channels),
    velocity(*this, enc_channels),
    watchdog(*this, milliseconds(100))
{
    ++next_id_;
    if (open)
        Q8Usb::open();
}

Q8Usb::~Q8Usb() {
    // set default options on program end
    set_options(QOptions());
    // if enabled, disable
    if (enabled_)
        disable();
    // if open, close
    if (open_)
        close();
    // decrement next_id_
    --next_id_;
}

bool Q8Usb::open() {
    // open as QDaq
    if (!QDaq::open())
        return false;
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // sanity check
    if (perform_sanity_check_) {
        print("Sanity checking " + namify(name_) + " ... ", false);
        if (!sanity_check()) {
            print("Failed. Reopening the device.");
            close();
            return open();
        }
    }
    print("Passed");

    // set default expire values (digital = LOW, analog = 0.0V)
    if (!analog_output.set_expire_values(std::vector<voltage>(8, 0.0))) {
        close();
        return false;
    }
    if (!digital_output.set_expire_values(std::vector<logic>(8, LOW))) {
        close();
        return false;
    }
    // allow changes to take effect
    sleep(milliseconds(10));
    return true;
}

bool Q8Usb::close() {
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    // close as QDaq
    return QDaq::close();
}

bool Q8Usb::enable() {
    if (!open_) {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
    print("Enabling " + namify(name_) + " ... ");
    // enable each module
    if (!analog_input.enable())
        return false;
    if (!analog_output.enable())
        return false;
    if (!digital_input.enable())
        return false;
    if (!digital_output.enable())
        return false;
    if (!encoder.enable())
        return false;
    if (!velocity.enable())
        return false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::enable();
}

bool Q8Usb::disable() {
    if (!open_) {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
    print("Disabling " + namify(name_) + " ... ");
    // disable each module
    if (!analog_input.disable())
        return false;
    if (!analog_output.disable())
        return false;
    if (!digital_input.disable())
        return false;
    if (!digital_output.disable())
        return false;
    if (!encoder.disable())
        return false;
    if (!velocity.disable())
        return false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::disable();
}

bool Q8Usb::update_input() {
    if (!open_) {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
    t_error result;
    result = hil_read(handle_,
        analog_input.get_channel_count() > 0 ? &(analog_input.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(analog_input.get_channel_count()),
        encoder.get_channel_count() > 0 ? &(encoder.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(encoder.get_channel_count()),
        digital_input.get_channel_count() > 0 ? &(digital_input.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(digital_input.get_channel_count()),
        velocity.get_channel_count() > 0 ? &(velocity.get_converted_channel_numbers())[0] : NULL,
        static_cast<uint32>(velocity.get_channel_count()),
        analog_input.get_channel_count() > 0 ? &(analog_input.get_values())[0] : NULL,
        encoder.get_channel_count() > 0 ? &(encoder.get_values())[0] : NULL,
        digital_input.get_channel_count() > 0 ? &(digital_input.get_values())[0] : NULL,
        velocity.get_channel_count() > 0 ? &(velocity.get_values())[0] : NULL);
    if (result == 0)
        return true;
    else {
        print(QDaq::get_quanser_error_message(result));
        return false;
    }
}

bool Q8Usb::update_output() {
    if (!open_) {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
    t_error result;
    result = hil_write(handle_,
        analog_output.get_channel_count() > 0 ? &(analog_output.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(analog_output.get_channel_count()),
        NULL, 0,
        digital_output.get_channel_count() > 0 ? &(digital_output.get_channel_numbers())[0] : NULL,
        static_cast<uint32>(digital_output.get_channel_count()),
        NULL, 0,
        analog_output.get_channel_count() > 0 ? &(analog_output.get_values())[0] : NULL,
        NULL,
        digital_output.get_channel_count() > 0 ? &(digital_output.get_values())[0] : NULL,
        NULL);
    if (result == 0)
        return true;
    else {
        print(QDaq::get_quanser_error_message(result));
        return false;
    }
}

bool Q8Usb::identify(uint32 channel_number) {
    if (!open_) {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
    Input<logic>::Channel di_ch = digital_input.get_channel(channel_number);
    Output<logic>::Channel do_ch = digital_output.get_channel(channel_number);
    for (int i = 0; i < 5; ++i) {
        do_ch.set_value(HIGH);
        do_ch.update();
        sleep(milliseconds(10));
        di_ch.update();
        if (di_ch.get_value() != HIGH) {
            return false;
        }
        do_ch.set_value(LOW);
        do_ch.update();
        sleep(milliseconds(10));
        di_ch.update();
        if (di_ch.get_value() != LOW) {
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
    velocity.update();
    bool sane = true;
    std::vector<double> velocities = velocity.get_values();
    for (auto it = velocities.begin(); it != velocities.end(); ++it) {
        if (*it != 0.0) {
            sane = false;
            break;
        }
    }
    return sane;
}

std::size_t Q8Usb::get_q8_usb_count() {
    return QDaq::get_qdaq_count("q8_usb");
}

} // namespace mel

