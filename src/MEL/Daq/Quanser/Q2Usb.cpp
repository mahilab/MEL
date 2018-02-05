#include <MEL/Daq/Quanser/Q2Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// STATIC MEMBER VARIABLES
//==============================================================================

 uint32 Q2Usb::next_id_ = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Q2Usb::Q2Usb(QOptions options, bool open, uint32 id) :
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
    // increment next_id_
    ++next_id_;
    // add modules
    add_module(static_cast<AnalogInput*>(&analog_input));
    add_module(static_cast<AnalogOutput*>(&analog_output));
    add_module(static_cast<DigitalInputOutput*>(&digital_io));
    add_module(static_cast<Encoder*>(&encoder));
    // if open true, open automatically
    if (open)
        Q2Usb::open();
}


Q2Usb::~Q2Usb() {

    // if enabled, disable
    if (is_enabled())
        disable();
    // if open, close
    if (open_) {
        // set default options on program end
        set_options(QOptions());
        close();
    }
    // decrement next_id_
    --next_id_;
}

bool Q2Usb::open() {
    // open as QDaq
    if (!QDaq::open())
        return false;
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // set default expire values (digital = LOW, analog = 0.0V)
    if (!analog_output.set_expire_values(std::vector<Voltage>(2, 0.0))) {
        close();
        return false;
    }
    if (!digital_io.set_expire_values(std::vector<Logic>(9, Low))) {
        close();
        return false;
    }
    // allow changes to take effect
    sleep(milliseconds(10));
    return true;
}

bool Q2Usb::close() {
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    // close as QDaq
    return QDaq::close();
}

bool Q2Usb::enable() {
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    // enable each module
    if (!analog_input.enable())
        return false;
    if (!analog_output.enable())
        return false;
    if (!digital_io.enable())
        return false;
    if (!encoder.enable())
        return false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::enable();
}

bool Q2Usb::disable() {
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
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
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    if (analog_input.update() && encoder.update() && digital_io.update())
        return true;
    else {
        return false;
    }
}

bool Q2Usb::update_output() {
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    if (analog_output.update() && digital_io.update())
        return true;
    else
        return false;
}

bool Q2Usb::identify(uint32 input_channel_number, uint32 outout_channel_number) {
    if (!open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << get_name() << " is not open";
        return false;
    }
    InputOutput<Logic>::Channel di_ch = digital_io.get_channel(input_channel_number);
    InputOutput<Logic>::Channel do_ch = digital_io.get_channel(outout_channel_number);
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
    digital_io[8].set_value(value);
}

std::size_t Q2Usb::get_q2_usb_count() {
    return QDaq::get_qdaq_count("q2_usb");
}

} // namespace mel

