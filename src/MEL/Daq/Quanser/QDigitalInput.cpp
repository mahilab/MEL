#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QDigitalInput.hpp>
#include <hil.h>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QDigitalInput::QDigitalInput(QDaq& daq, const std::vector<uint32>& channel_numbers) :
        Input(daq.name_ + "_digital_input", channel_numbers),
        daq_(daq)
    {
    }

    QDigitalInput::~QDigitalInput() {

    }

    bool QDigitalInput::enable() {
        if (enabled_)
            return true;
        print("Enabling " + namify(name_) + " ... Done");
        return Device::enable();
    }

    bool QDigitalInput::disable() {
        if (!enabled_)
            return true;
        print("Disabling " + namify(name_) + " ... Done");
        return Device::disable();
    }

    bool QDigitalInput::update() {
        if (daq_.open_) {
            t_error result;
            result = hil_read_digital(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
            if (result == 0)
                return true;
            else {
                print(QDaq::get_quanser_error_message(result));
                return false;
            }
        }
        else {
            print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
            return false;
        }
    }

    bool QDigitalInput::update_channel(uint32 channel_number) {
        if (daq_.open_) {
            t_error result;
            result = hil_read_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
            if (result == 0)
                return true;
            else {
                print(QDaq::get_quanser_error_message(result));
                return false;
            }
        }
        else {
            print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
            return false;
        }
    }

} // namespace mel