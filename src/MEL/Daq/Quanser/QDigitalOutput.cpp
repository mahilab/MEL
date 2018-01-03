#include <MEL/Daq/Quanser/QDigitalOutput.hpp>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <hil.h>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QDigitalOutput::QDigitalOutput(QDaq& daq, const std::vector<uint32>& channel_numbers) :
        OutputModule(daq.name_ + "_digital_output", channel_numbers),
        daq_(daq)
    {
    }

    QDigitalOutput::~QDigitalOutput() {

    }

    bool QDigitalOutput::enable() {
        print("Enabling " + namify(name_) + " ... ", false);
        set_values(enable_values_);
        if (update()) {
            print("Done");
            return Device::enable();
        }
        print("Failed");
        return false;
    }

    bool QDigitalOutput::disable() {
        print("Disabling " + namify(name_) + " ... ", false);
        set_values(disable_values_);
        if (update()) {
            print("Done");
            return Device::disable();
        }
        print("Failed");
        return false;
    }

    bool QDigitalOutput::update() {
        if (daq_.open_) {
            t_error result;
            result = hil_write_digital(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
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

    bool QDigitalOutput::update_channel(uint32 channel_number) {
        if (daq_.open_) {
            t_error result;
            result = hil_write_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
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

    bool QDigitalOutput::set_expire_values(const std::vector<logic>& expire_values) {
        if (!OutputModule::set_expire_values(expire_values))
            return false;
        if (daq_.open_) {
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

    bool QDigitalOutput::set_expire_value(uint32 channel_number, logic expire_value) {
        if (!OutputModule::set_expire_value(channel_number, expire_value))
            return false;
        if (daq_.open_) {
            // convert MEL logic to Quanser t_encoder_quadratue_mode
            t_digital_state converted_expire_value;
            if (expire_value == HIGH)
                converted_expire_value = DIGITAL_STATE_HIGH;
            else
                converted_expire_value = DIGITAL_STATE_LOW;
            t_error result;
            result = hil_watchdog_set_digital_expiration_state(daq_.handle_, &channel_number, static_cast<uint32>(1), &converted_expire_value);
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