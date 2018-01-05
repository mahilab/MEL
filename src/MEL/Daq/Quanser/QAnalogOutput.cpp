#include <MEL/Daq/Quanser/QAnalogOutput.hpp>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Utility/System.hpp>
#include <hil.h>

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
        return true;
    print("Enabling " + namify(name_) + " ... ", false);
    set_values(enable_values_);
    if (update()) {
        print("Done");
        return Device::enable();
    }
    print("Failed");
    return false;
}

bool QAnalogOutput::disable() {
    if (!enabled_)
        return true;
    print("Disabling " + namify(name_) + " ... ", false);
    set_values(disable_values_);
    if (update()) {
        print("Done");
        return Device::disable();
    }
    print("Failed");
    return false;
}

bool QAnalogOutput::update() {
    if (daq_.open_) {
        t_error result;
        result = hil_write_analog(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
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

bool QAnalogOutput::update_channel(uint32 channel_number) {
    if (daq_.open_) {
        t_error result;
        result = hil_write_analog(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
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

bool QAnalogOutput::set_ranges(const std::vector<voltage>& min_values, const std::vector<voltage>& max_values) {
    if (!Module::set_ranges(min_values, max_values))
        return false;
    if (daq_.open_) {
        t_error result;
        result = hil_set_analog_output_ranges(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &min_values_[0], &max_values_[0]);
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

bool QAnalogOutput::set_range(uint32 channel_number, voltage min_value, voltage max_value) {
    if (!Module::set_range(channel_number, min_value, max_value))
        return false;
    if (daq_.open_) {
        t_error result;
        result = hil_set_analog_output_ranges(daq_.handle_, &channel_number, static_cast<uint32>(1), &min_values_[channel_map_.at(channel_number)], &max_values_[channel_map_.at(channel_number)]);
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

bool QAnalogOutput::set_expire_values(const std::vector<voltage>& expire_values) {
    if (!Output::set_expire_values(expire_values))
        return false;
    if (daq_.open_) {
        t_error result;
        result = hil_watchdog_set_analog_expiration_state(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &expire_values_[0]);
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

bool QAnalogOutput::set_expire_value(uint32 channel_number, voltage expire_value) {
    if (!Output::set_expire_value(channel_number, expire_value))
        return false;
    if (daq_.open_) {
        t_error result;
        result = hil_watchdog_set_analog_expiration_state(daq_.handle_, &channel_number, static_cast<uint32>(1), &expire_values_[channel_map_.at(channel_number)]);
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