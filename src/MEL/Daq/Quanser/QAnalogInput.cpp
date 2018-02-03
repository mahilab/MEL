#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QAnalogInput.hpp>
#include <MEL/Utility/Log.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QAnalogInput::QAnalogInput(QDaq& daq, const std::vector<uint32>& channel_numbers) :
    Input(daq.name_ + "_analog_input", channel_numbers),
    daq_(daq)
{
}

QAnalogInput::~QAnalogInput() {

}

bool QAnalogInput::enable() {
    return Device::enable();
}

bool QAnalogInput::disable() {
    return Device::disable();
}

bool QAnalogInput::update() {
    if (!daq_.open_) {
        LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because <"
                   << daq_.get_name() << "> is not open";
        return false;
    }
    t_error result;
    result = hil_read_analog(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
    if (result == 0)
        return true;
    else {
        LOG(ERROR) << "Failed to update <" << name_ << "> "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogInput::update_channel(uint32 channel_number) {
    if (!daq_.open_) {
        LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because <"
                   << daq_.get_name() << "> is not open";
        return false;
    }
    t_error result;
    result = hil_read_analog(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
    if (result == 0)
        return true;
    else {
        LOG(ERROR) << "Failed to update <" << name_ << "> channel number " << channel_number << " "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogInput::set_ranges(const std::vector<voltage>& min_values, const std::vector<voltage>& max_values) {
    if (!daq_.open_) {
        LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because <"
            << daq_.get_name() << "> is not open";
        return false;
    }
    if (!Module::set_ranges(min_values, max_values))
        return false;
    t_error result;
    result = hil_set_analog_input_ranges(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &min_values_[0], &max_values_[0]);
    if (result == 0) {
        LOG(INFO) << "Set <" << name_ << "> ranges to min=" << min_values << ", max=" << max_values;
        return true;
    }
    else {
        LOG(ERROR) << "Failed to set <" << name_ << "> ranges "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogInput::set_range(uint32 channel_number, voltage min_value, voltage max_value) {
    if (!daq_.open_) {
        LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because <"
            << daq_.get_name() << "> is not open";
        return false;
    }
    if (!Module::set_range(channel_number, min_value, max_value))
        return false;
    t_error result;
    result = hil_set_analog_input_ranges(daq_.handle_, &channel_number, static_cast<uint32>(1), &min_values_[channel_map_.at(channel_number)], &max_values_[channel_map_.at(channel_number)]);
    if (result == 0) {
        LOG(INFO) << "Set <" << name_ << "> channel number " << channel_number << " range to min=" << min_value << ", max=" << max_value;
        return true;
    }
    else {
        LOG(ERROR) << "Failed to set <" << name_ << "> channel number " << channel_number << " range "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

} // namespace mel
