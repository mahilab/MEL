#include <hil.h>
#include <MEL/Daq/Quanser/QuanserDaq.hpp>
#include <MEL/Daq/Quanser/QuanserDIO.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QuanserDIO::QuanserDIO(QuanserDaq& daq) :
        daq_(daq)
    {
        set_name(daq_.get_name() + "_DIO");
    }

    QuanserDIO::~QuanserDIO() {

    }

    bool QuanserDIO::enable() {
        if (is_enabled())
            return Device::enable();
        set_values(enable_values_.get());
        if (update()) {
            LOG(Verbose) << "Set " << get_name() << " enable values to " << enable_values_;
            return Device::enable();
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " enable values to " << enable_values_;
            return false;
        }
    }

    bool QuanserDIO::disable() {
        if (!is_enabled())
            return Device::disable();
        set_values(disable_values_.get());
        if (update()) {
            LOG(Verbose) << "Set " << get_name() << " disable values to " << disable_values_;
            return Device::disable();
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " disable values to " << disable_values_;
            return false;
        }
    }

    bool QuanserDIO::update() {
        // create intermediate buffers
        std::vector<char> read_buffer(input_channel_numbers_.size());
        std::vector<char> write_buffer(output_channel_numbers_.size());
        for (std::size_t i = 0; i < output_channel_numbers_.size(); ++i)
            write_buffer[i] = static_cast<char>(values_[output_channel_numbers_[i]]);
        t_error result;
        result = hil_read_digital_write_digital(daq_.handle_,
            &input_channel_numbers_[0], static_cast<uint32>(input_channel_numbers_.size()),
            &output_channel_numbers_[0], static_cast<uint32>(output_channel_numbers_.size()),
            &read_buffer[0], &write_buffer[0]);
        for (std::size_t i = 0; i < input_channel_numbers_.size(); ++i)
            values_[input_channel_numbers_[i]] = static_cast<Logic>(read_buffer[i]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::update_channel(uint32 channel_number) {
        char buffer;
        t_error result;
        if (directions_[channel_number] == In) {
            result = hil_read_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &buffer);
            values_[channel_number] = static_cast<Logic>(buffer);
        }
        else {
            buffer = static_cast<char>(values_[channel_number]);
            result = hil_write_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &buffer);
        }
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::set_directions(const std::vector<Direction>& directions) {
        if (!InputOutput::set_directions(directions))
            return false;
        t_error result;
        result = hil_set_digital_directions(daq_.handle_,
            &input_channel_numbers_[0], static_cast<uint32>(input_channel_numbers_.size()),
            &output_channel_numbers_[0], static_cast<uint32>(output_channel_numbers_.size()));
        if (result == 0) {
            LOG(Verbose) << "Set " << get_name() << " directions";
            return true;
        }
        else {
            LOG(Error) << "Failed to set " << get_name() << " directions "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDIO::set_direction(uint32 channel_number, Direction direction) {
        if (!InputOutput::set_direction(channel_number, direction))
            return false;
        return set_directions(directions_.get());
    }

    bool QuanserDIO::set_expire_values(const std::vector<Logic>& expire_values) {
        if (!InputOutput::set_expire_values(expire_values))
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

    bool QuanserDIO::set_expire_value(uint32 channel_number, Logic expire_value) {
        if (!InputOutput::set_expire_value(channel_number, expire_value))
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
