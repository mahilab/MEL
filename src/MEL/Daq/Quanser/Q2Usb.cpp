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
    digital_io(*this, {0, 1, 2, 3, 4, 5, 6, 7, 8},
                      { QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Input,
                        QDigitalInputOutput::Direction::Output }
                      ),
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
    if (!digital_io.enable())
        return false;
    if (!encoder.enable())
        return false;
    // set default expire values (digital = LOW, analog = 0.0V)
    if (!analog_output.set_expire_values(std::vector<voltage>(2, 0.0)))
        return false;
    if (!digital_io.set_expire_values(std::vector<logic>(9, LOW)))
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
    if (!digital_io.disable())
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

        if (analog_input.update() && encoder.update() && digital_io.update())
            return true;
        else {
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
        if (analog_output.update() && digital_io.update())
            return true;
    }
    else {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool Q2Usb::identify(uint32 input_channel_number, uint32 outout_channel_number) {
    if (open_) {
        InputOutput<logic>::Channel di_ch = digital_io.get_channel(input_channel_number);
        InputOutput<logic>::Channel do_ch = digital_io.get_channel(outout_channel_number);
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

void Q2Usb::set_led(logic value) {
    digital_io[8].set_value(value);
}

std::size_t Q2Usb::get_q2_usb_count() {
    return QDaq::get_qdaq_count("q2_usb");
}

} // namespace mel

