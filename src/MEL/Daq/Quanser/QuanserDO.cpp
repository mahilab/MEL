#include <hil.h>
#include <MEL/Daq/Quanser/QuanserDaq.hpp>
#include <MEL/Daq/Quanser/QuanserDO.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QuanserDO::QuanserDO(QuanserDaq& daq, const std::vector<uint32>& channel_numbers) :
        DigitalOutput(channel_numbers),
        daq_(daq),
        quanser_values_(this)
    {
        set_name(daq.get_name() + "_DO");
    }

    bool QuanserDO::update() {
        // convert MEL Logic to Quanser t_boolean (aka char)
        for (auto const& ch : get_channel_numbers())
            quanser_values_[ch] = static_cast<char>(values_[ch]);
        t_error result;
        result = hil_write_digital(daq_.handle_, &get_channel_numbers()[0], static_cast<uint32>(get_channel_count()), &quanser_values_.get()[0]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDO::update_channel(uint32 channel_number) {
        // convert MEL Logic to Quanser t_boolean (aka char)
        quanser_values_[channel_number] = static_cast<char>(values_[channel_number]);
        t_error result;
        result = hil_write_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &quanser_values_[channel_number]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }


    std::vector<char>& QuanserDO::get_quanser_values() {
        return quanser_values_.get();
    }

    bool QuanserDO::set_expire_values(const std::vector<Logic>& expire_values) {
        if (!Output::set_expire_values(expire_values))
            return false;
        // convert MEL logic to Quanser t_encoder_quadratue_mode
        std::vector<t_digital_state> converted_expire_values;
        for (auto it = expire_values.begin(); it != expire_values.end(); ++it) {
            if (*it == High)
                converted_expire_values.push_back(DIGITAL_STATE_HIGH);
            else
                converted_expire_values.push_back(DIGITAL_STATE_LOW);
        }
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &get_channel_numbers()[0], static_cast<uint32>(get_channel_count()), &converted_expire_values[0]);
        if (result == 0) {
            LOG(Verbose) << "Set " << get_name() << " expire values to " << expire_values_;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " expire values "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDO::set_expire_value(uint32 channel_number, Logic expire_value) {
        if (!Output::set_expire_value(channel_number, expire_value))
            return false;
        // convert MEL logic to Quanser t_encoder_quadratue_mode
        t_digital_state converted_expire_value;
        if (expire_value == High)
            converted_expire_value = DIGITAL_STATE_HIGH;
        else
            converted_expire_value = DIGITAL_STATE_LOW;
        t_error result;
        result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_number, static_cast<uint32>(1), &converted_expire_value);
        if (result == 0) {
            LOG(Verbose) << "Set " << get_name() << " channel number " << channel_number << " expire value to " << expire_value;
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " channel number " << channel_number << " expire value "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

} // namespace mel
