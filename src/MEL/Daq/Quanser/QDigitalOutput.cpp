#include <hil.h>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QDigitalOutput.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QDigitalOutput::QDigitalOutput(QDaq& daq, const std::vector<uint32>& channel_numbers) :
        Output(daq.name_ + "_digital_output", channel_numbers),
        daq_(daq)
    {
    }

    QDigitalOutput::~QDigitalOutput() {

    }

    bool QDigitalOutput::enable() {
        if (enabled_)
            return Device::enable();
        set_values(enable_values_);
        if (update()) {
            LOG(INFO) << "Set " << name_ << " enable values to " << enable_values_;
            return Device::enable();
        }
        else {
            LOG(ERROR) << "Failed to set " << name_ << " enable values to " << enable_values_;
            return false;
        }
    }

    bool QDigitalOutput::disable() {
        if (!enabled_)
            return Device::disable();
        set_values(disable_values_);
        if (update()) {
            LOG(INFO) << "Set " << name_ << " disable values to " << disable_values_;
            return Device::disable();
        }
        else {
            LOG(ERROR) << "Failed to set " << name_ << " disable values to " << disable_values_;
            return false;
        }
    }

    bool QDigitalOutput::update() {
        if (!daq_.open_) {
            LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        t_error result;
        result = hil_write_digital(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
        if (result == 0)
            return true;
        else {
            LOG(ERROR) << "Failed to update " << name_ << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QDigitalOutput::update_channel(uint32 channel_number) {
        if (!daq_.open_) {
            LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        t_error result;
        result = hil_write_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
        if (result == 0)
            return true;
        else {
            LOG(ERROR) << "Failed to update " << name_ << " channel number " << channel_number << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QDigitalOutput::set_expire_values(const std::vector<logic>& expire_values) {
        if (!Output::set_expire_values(expire_values))
            return false;
        if (!daq_.open_) {
            LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        // convert MEL logic to Quanser t_encoder_quadratue_mode
        std::vector<t_digital_state> converted_expire_values;
        for (auto it = expire_values.begin(); it != expire_values.end(); ++it) {
            if (*it == HIGH)
                converted_expire_values.push_back(DIGITAL_STATE_HIGH);
            else
                converted_expire_values.push_back(DIGITAL_STATE_LOW);
        }
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &converted_expire_values[0]);
        if (result == 0) {
            LOG(INFO) << "Set " << name_ << " expire values to " << expire_values_;
            return true;
        }
        else {
            LOG(ERROR) << "Failed to set " << name_ << " expire values "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QDigitalOutput::set_expire_value(uint32 channel_number, logic expire_value) {
        if (!Output::set_expire_value(channel_number, expire_value))
            return false;
        if (!daq_.open_) {
            LOG(ERROR) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        // convert MEL logic to Quanser t_encoder_quadratue_mode
        t_digital_state converted_expire_value;
        if (expire_value == HIGH)
            converted_expire_value = DIGITAL_STATE_HIGH;
        else
            converted_expire_value = DIGITAL_STATE_LOW;
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_number, static_cast<uint32>(1), &converted_expire_value);
        if (result == 0) {
            LOG(INFO) << "Set " << name_ << " channel number " << channel_number << " expire value to " << expire_value;
            return true;
        }
        else {
            LOG(ERROR) << "Failed to set " << name_ << " channel number " << channel_number << " expire value "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

} // namespace mel
