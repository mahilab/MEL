#include <hil.h>
#include <MEL/Daq/Quanser/QAnalogOutput.hpp>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QAnalogOutput::QAnalogOutput(QDaq& daq, const std::vector<uint32>& channel_numbers) :
    Output(daq.name_ + "_analog_output", channel_numbers),
    daq_(daq)
{
}

QAnalogOutput::~QAnalogOutput() {

}

bool QAnalogOutput::enable() {
    if (enabled_)
        return Device::enable();
    set_values(enable_values_);
    if (update()) {
        LOG(Info) << "Set " << name_ << " enable values to " << enable_values_;
        return Device::enable();
    }
    else {
        LOG(Error) << "Failed to set " << name_ << " enable values to " << enable_values_;
        return false;
    }
}

bool QAnalogOutput::disable() {
    if (!enabled_)
        return Device::disable();
    set_values(disable_values_);
    if (update()) {
        LOG(Info) << "Set " << name_ << " disable values to " << disable_values_;
        return Device::disable();
    }
    else {
        LOG(Error) << "Failed to set " << name_ << " disable values to " << disable_values_;
        return false;
    }
}

bool QAnalogOutput::update() {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_write_analog(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << name_ << " "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogOutput::update_channel(uint32 channel_number) {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_write_analog(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << name_ << " channel number " << channel_number << " "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogOutput::set_ranges(const std::vector<Voltage>& min_values, const std::vector<Voltage>& max_values) {
    if (!Module::set_ranges(min_values, max_values))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_set_analog_output_ranges(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &min_values_[0], &max_values_[0]);
    if (result == 0) {
        LOG(Info) << "Set " << name_ << " ranges to min=" << min_values << ", max=" << max_values;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << name_ << " ranges "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogOutput::set_range(uint32 channel_number, Voltage min_value, Voltage max_value) {
    if (!Module::set_range(channel_number, min_value, max_value))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_set_analog_output_ranges(daq_.handle_, &channel_number, static_cast<uint32>(1), &min_values_[channel_map_.at(channel_number)], &max_values_[channel_map_.at(channel_number)]);
    if (result == 0) {
        LOG(Info) << "Set " << name_ << " channel number " << channel_number << " range to min=" << min_value << ", max=" << max_value;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << name_ << " channel number " << channel_number << " range "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogOutput::set_expire_values(const std::vector<Voltage>& expire_values) {
    if (!Output::set_expire_values(expire_values))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_watchdog_set_analog_expiration_state(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &expire_values_[0]);
    if (result == 0) {
        LOG(Info) << "Set " << name_ << " expire values to " << expire_values_;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << name_ << " expire values "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QAnalogOutput::set_expire_value(uint32 channel_number, Voltage expire_value) {
    if (!Output::set_expire_value(channel_number, expire_value))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_watchdog_set_analog_expiration_state(daq_.handle_, &channel_number, static_cast<uint32>(1), &expire_values_[channel_map_.at(channel_number)]);
    if (result == 0) {
        LOG(Info) << "Set " << name_ << " channel number " << channel_number << " expire value to " << expire_value;
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << name_ << " channel number " << channel_number << " expire value "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

} // namespace mel
