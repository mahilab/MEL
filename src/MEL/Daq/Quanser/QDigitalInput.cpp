#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QDigitalInput.hpp>
#include <MEL/Logging/Log.hpp>
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
        return Device::enable();
    }

    bool QDigitalInput::disable() {
        return Device::disable();
    }

    bool QDigitalInput::update() {
        if (!daq_.open_) {
            LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        t_error result;
        result = hil_read_digital(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
        if (result == 0)
            return true;
        else {
            LOG(ERROR) << "Failed to update " << name_ << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QDigitalInput::update_channel(uint32 channel_number) {
        if (!daq_.open_) {
            LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        t_error result;
        result = hil_read_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
        if (result == 0)
            return true;
        else {
            LOG(ERROR) << "Failed to update " << name_ << " channel number " << channel_number << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

} // namespace mel
