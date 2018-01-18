#include <MEL/Daq/Quanser/Q2Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// STATIC MEMBER VARIABLES
//==============================================================================

 uint32 Q2Usb::next_id_ = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Q2Usb::Q2Usb(QOptions options, uint32 id) :
    QDaq("q2_usb", id, options),
    analog_input(*this, {0, 1}),
    analog_output(*this, {0, 1}),
    digital_input(*this, {0, 1}),
    digital_output(*this, {0, 1}),
    encoder(*this, {0, 1}),
    watchdog(*this, milliseconds(100))
{
    ++next_id_;
}


Q2Usb::~Q2Usb() {
    if (enabled_)
        disable();
    if (open_)
        close();
    --next_id_;
}

bool Q2Usb::enable() {
    // open if not already
    if (!open_)
        if (!open())
            return false;
    print("Enabling " + namify(name_) + " ... ");
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // set options
    if (!set_options())
        return false;
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
    // set default expire values (digital = LOW, analog = 0.0V)
    if (!analog_output.set_expire_values(std::vector<voltage>(8, 0.0)))
        return false;
    if (!digital_output.set_expire_values(std::vector<logic>(8, LOW)))
        return false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::enable();
}

bool Q2Usb::disable() {
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
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::disable();
}

bool Q2Usb::update_input() {
    if (open_) {
        t_error result;
        result = hil_read(handle_,
            analog_input.get_channel_count() > 0 ? &(analog_input.get_channel_numbers())[0] : NULL,
            static_cast<uint32>(analog_input.get_channel_count()),
            encoder.get_channel_count() > 0 ? &(encoder.get_channel_numbers())[0] : NULL,
            static_cast<uint32>(encoder.get_channel_count()),
            digital_input.get_channel_count() > 0 ? &(digital_input.get_channel_numbers())[0] : NULL,
            static_cast<uint32>(digital_input.get_channel_count()),
            NULL,
            0,
            analog_input.get_channel_count() > 0 ? &(analog_input.get_values())[0] : NULL,
            encoder.get_channel_count() > 0 ? &(encoder.get_values())[0] : NULL,
            digital_input.get_channel_count() > 0 ? &(digital_input.get_values())[0] : NULL,
            NULL);
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool Q2Usb::update_output() {
    if (open_) {
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
    else {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool Q2Usb::identify(uint32 channel_number) {
    if (open_) {
        Input<logic>::Channel di_ch = digital_input.get_channel(channel_number);
        Output<logic>::Channel do_ch = digital_output.get_channel(channel_number);
        bool loopback_detected = true;
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
    else {
        print(namify(name_) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

int Q2Usb::identify() {
    for (uint32 channel_number = 0; channel_number < 8; ++channel_number) {
        if (identify(channel_number))
            return (int)channel_number;
    }
    return -1;
}

std::size_t Q2Usb::get_q2_usb_count() {
    return QDaq::get_qdaq_count("q2_usb");
}

} // namespace mel

