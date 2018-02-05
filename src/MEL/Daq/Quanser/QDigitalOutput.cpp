#include <hil.h>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QDigitalOutput.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QDigitalOutput::QDigitalOutput(QDaq& daq, const std::vector<uint32>& channel_numbers) :
        Output(daq.get_name() + "_digital_output", channel_numbers),
        daq_(daq),
        quanser_values_(channel_count_, char(0)) 
    {
    }

    QDigitalOutput::~QDigitalOutput() {

    }

    bool QDigitalOutput::enable() {
        if (is_enabled())
            return Device::enable();
        set_values(enable_values_);
        if (update()) {
            LOG(Info) << "Set " << get_name() << " enable values to " << enable_values_;
            return Device::enable();
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " enable values to " << enable_values_;
            return false;
        }
    }

    bool QDigitalOutput::disable() {
        if (!is_enabled())
            return Device::disable();
        set_values(disable_values_);
        if (update()) {
            LOG(Info) << "Set " << get_name() << " disable values to " << disable_values_;
            return Device::disable();
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " disable values to " << disable_values_;
            return false;
        }
    }

    bool QDigitalOutput::update() {
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        // convert MEL Logic to Quanser t_boolean (aka char)
        for (std::size_t i = 0; i < channel_count_; ++i)
            quanser_values_[i] = static_cast<char>(values_[i]);
        t_error result;
        result = hil_write_digital(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &quanser_values_[0]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QDigitalOutput::update_channel(uint32 channel_number) {
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        // convert MEL Logic to Quanser t_boolean (aka char)
        quanser_values_[channel_map_.at(channel_number)] = static_cast<char>(values_[channel_map_.at(channel_number)]);
        t_error result;
        result = hil_write_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &quanser_values_[channel_map_.at(channel_number)]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }


    std::vector<char>& QDigitalOutput::get_quanser_values() {
        return quanser_values_;
    }

    bool QDigitalOutput::set_expire_values(const std::vector<Logic>& expire_values) {
        if (!Output::set_expire_values(expire_values))
            return false;
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        // convert MEL logic to Quanser t_encoder_quadratue_mode
        std::vector<t_digital_state> converted_expire_values;
        for (auto it = expire_values.begin(); it != expire_values.end(); ++it) {
            if (*it == High)
                converted_expire_values.push_back(DIGITAL_STATE_HIGH);
            else
                converted_expire_values.push_back(DIGITAL_STATE_LOW);
        }
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &converted_expire_values[0]);
        if (result == 0) {
            LOG(Info) << "Set " << get_name() << " expire values to " << expire_values_;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " expire values "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QDigitalOutput::set_expire_value(uint32 channel_number, Logic expire_value) {
        if (!Output::set_expire_value(channel_number, expire_value))
            return false;
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        // convert MEL logic to Quanser t_encoder_quadratue_mode
        t_digital_state converted_expire_value;
        if (expire_value == High)
            converted_expire_value = DIGITAL_STATE_HIGH;
        else
            converted_expire_value = DIGITAL_STATE_LOW;
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_number, static_cast<uint32>(1), &converted_expire_value);
        if (result == 0) {
            LOG(Info) << "Set " << get_name() << " channel number " << channel_number << " expire value to " << expire_value;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " channel number " << channel_number << " expire value "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

} // namespace mel
