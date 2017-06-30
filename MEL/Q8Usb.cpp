#include "Q8Usb.h"

namespace mel {

    Q8Usb::Q8Usb(std::string id,
        uint_vec ai_channels,
        uint_vec ao_channels,
        uint_vec di_channels,
        uint_vec do_channels,
        uint_vec enc_channels,
        char * options) :
        Daq("q8_usb", id,
            ai_channels,
            ao_channels,
            di_channels,
            do_channels,
            enc_channels,
            get_q8_velocity_channels(enc_channels))
    {
        /* set up analog input channels */
        ai_min_voltages_ = double_vec(ai_channels_nums_.size(), ai_min_voltage_);
        ai_max_voltages_ = double_vec(ai_channels_nums_.size(), ai_max_voltage_);

        /* set up analog output channels */
        ao_min_voltages_ = double_vec(ao_channels_nums_.size(), ao_min_voltage_);
        ao_max_voltages_ = double_vec(ao_channels_nums_.size(), ao_max_voltage_);
        ao_initial_voltages_ = double_vec(ao_channels_nums_.size(), ao_initial_voltage_);
        ao_final_voltages_ = double_vec(ao_channels_nums_.size(), ao_final_voltage_);
        ao_exp_voltages_ = double_vec(ao_channels_nums_.size(), ao_exp_voltage_);

        /* set up digital output channels */
        do_initial_states_ = char_vec(do_channels_nums_.size(), do_initial_state_);
        do_final_states_ = char_vec(do_channels_nums_.size(), do_final_state_);
        do_exp_states_ = std::vector<t_digital_state>(do_channels_nums_.size(), do_exp_state_);

        /* set up encoder channels */
        enc_modes_ = std::vector<t_encoder_quadrature_mode>(enc_channels.size(), enc_mode_);

        /* set up options */
        strcpy(options_, options);
    }

    int Q8Usb::activate() {
        if (!active_) {
            t_error result;
            // Attempt to Open Q8 USB and Sanity Check Encoder Velocity Readings (10 attempts)
            std::cout << "Opening Q8 USB ... ";
            for (int attempt = 0; attempt < 10; attempt++) {
                result = hil_open("q8_usb", id_.c_str(), &q8_usb_);
                if (result == 0) {
                    double temp[3]; // TODO: FIX THIS CRAP
                    result = hil_read_other(q8_usb_, &encrate_channels_nums_[0], encoder_channels_nums_.size(), temp);
                    if (temp[0] == 0 && temp[1] == 0 && temp[2] == 0) {
                        std::cout << "Attempt " << attempt + 1 << ": Success" << std::endl;
                        std::cout << "Done" << std::endl;
                        break;
                    }
                    else {
                        std::cout << "Attempt " << attempt + 1 << ": Encoder Reading Errors" << std::endl;
                        result = 1;
                        hil_close(q8_usb_);
                    }
                }
                else {
                    std::cout << "Attempt " << attempt + 1 << ": Failed to Open" << std::endl;
                    print_quarc_error(result);
                }
            }

            // If all attempts were unsuccessful, display message and terminate the application.
            if (result != 0) {
                std::cout << "Failed" << std::endl;
                return 0;
            }

            active_ = true;

            // Configure Q8 USB (Functions called in same order as Simulink compiled code)
            std::cout << "Configuring Q8 USB ... ";
            result = hil_set_card_specific_options(q8_usb_, options_, strlen(options_)); // TODO: make this optional or configured in another way
            if (result < 0)
                print_quarc_error(result);

            // Stop and Clear Watchdog
            stop_watchdog();

            // set analog I/O ranges
            if (num_ai_channels_ > 0) {
                result = hil_set_analog_input_ranges(q8_usb_, &ai_channels_nums_[0], num_ai_channels_, &ai_min_voltages_[0], &ai_max_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            if (num_ao_channels_ > 0) {
                result = hil_set_analog_output_ranges(q8_usb_, &ao_channels_nums_[0], num_ao_channels_, &ao_min_voltages_[0], &ao_max_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }

            // set analog/digital output expiration states
            if (num_ao_channels_ > 0) {
                result = hil_watchdog_set_analog_expiration_state(q8_usb_, &ao_channels_nums_[0], num_ao_channels_, &ao_exp_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            if (num_do_channels_ > 0) {
                result = hil_watchdog_set_digital_expiration_state(q8_usb_, &do_channels_nums_[0], num_do_channels_, &do_exp_states_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }

            // set encoder quadrature modes
            if (num_enc_channels_ > 0) {
                result = hil_set_encoder_quadrature_mode(q8_usb_, &encoder_channels_nums_[0], num_enc_channels_, &enc_modes_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }

            // set and write initial voltages and states
            ao_voltages_ = ao_initial_voltages_;
            do_states_ = do_initial_states_;
            write_all();

            // TO BE IMPLEMENTED IN FUTURE (IN THIS ORDER):

            // hil_set_pwm_mode
            // hil_set_pwm_frequency
            // hil_set_pwm_duty_cycle
            // hil_set_pwm_configuration
            // hil_set_pwm_deadband
            // hil_write_pwm
            // hil_watchdog_set_pwm_expiration_state

            std::cout << "Done" << std::endl;
            return 1;
        }
    }

    void Q8Usb::zero_encoder_counts() {
        if (active_ && num_enc_channels_ > 0) {
            std::cout << "Zeroing Encoder Counts ... ";
            int_vec enc_zero_counts(num_enc_channels_, 0);
            t_error result = hil_set_encoder_counts(q8_usb_, &encoder_channels_nums_[0], encoder_channels_nums_.size(), &enc_zero_counts[0]);
            if (result != 0)
                print_quarc_error(result);
            std::cout << "Done" << std::endl;
        }
    }

    void Q8Usb::offset_encoder_counts(int_vec offset_counts) {
        if (active_ && num_enc_channels_ > 0) {
            std::cout << "Offsetting Encoder Counts ... ";
            offset_counts.resize(num_enc_channels_, 0);
            t_error result = hil_set_encoder_counts(q8_usb_, &encoder_channels_nums_[0], encoder_channels_nums_.size(), &offset_counts[0]);
            if (result != 0)
                print_quarc_error(result);
            std::cout << "Done" << std::endl;
        }
    }


    int Q8Usb::deactivate() {
        if (active_) {
            t_error result;

            // Stop all tasks and monitors (possibly unnecessary)
            hil_task_stop_all(q8_usb_);
            hil_monitor_stop_all(q8_usb_);

            // set and write final voltages and states
            ao_voltages_ = ao_final_voltages_;
            do_states_ = do_final_states_;
            write_all();            

            // Delete all tasks and monitors (possibly unnecessary)
            hil_task_delete_all(q8_usb_);
            hil_monitor_delete_all(q8_usb_);

            // Stop and Clear Watchdog
            stop_watchdog();

            // Close Q8 USB
            std::cout << "Closing Q8 USB ... ";
            result = hil_close(q8_usb_);
            if (result != 0) {
                std::cout << "Failed" << std::endl;
                print_quarc_error(result);
                return 0;
            }
            active_ = false;
            std::cout << "Done" << std::endl;
            return 1;
        }
        return 0;
    }

    void Q8Usb::read_analog() {
        if (active_ && num_ai_channels_ > 0) {
            t_error result = hil_read_analog(q8_usb_, &ai_channels_nums_[0], num_ai_channels_, &ai_voltages_[0]);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Either Q8 USB " << id_ << " has not been activated, or no analog input channels were passed to the constructor." << std::endl;
        }
    }

    void Q8Usb::read_digital() {
        if (active_ && num_di_channels_ > 0) {
            t_error result = hil_read_digital(q8_usb_, &di_channels_nums_[0], num_di_channels_, &di_states_[0]);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Either Q8 USB " << id_ << " has not been activated, or no digital input channels were passed to the constructor." << std::endl;
        }
    }

    void Q8Usb::read_encoder_counts() {
        if (active_ && num_enc_channels_ > 0) {
            t_error result = hil_read_encoder(q8_usb_, &encoder_channels_nums_[0], num_enc_channels_, &enc_counts_[0]);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Either Q8 USB " << id_ << " has not been activated, or no encoder channels were passed to the constructor." << std::endl;
        }
    }

    void Q8Usb::read_encoder_count_rates() {
        if (active_ && num_vel_channels_ > 0) {
            t_error result = hil_read_other(q8_usb_, &encrate_channels_nums_[0], num_vel_channels_, &enc_rates[0]);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Either Q8 USB " << id_ << "has not been activated, or no encoder channels were passed to the constructor." << std::endl;
        }
    }

    void Q8Usb::read_all() {
        if (active_) {
            t_error result = hil_read(q8_usb_,
                num_ai_channels_ > 0 ? &ai_channels_nums_[0] : NULL, num_ai_channels_,
                num_enc_channels_ > 0 ? &encoder_channels_nums_[0] : NULL, num_enc_channels_,
                num_di_channels_ > 0 ? &di_channels_nums_[0] : NULL, num_di_channels_,
                num_vel_channels_ > 0 ? &encrate_channels_nums_[0] : NULL, num_vel_channels_,
                num_ai_channels_ > 0 ? &ai_voltages_[0] : NULL,
                num_enc_channels_ > 0 ? &enc_counts_[0] : NULL,
                num_di_channels_ > 0 ? &di_states_[0] : NULL,
                num_vel_channels_ > 0 ? &enc_rates[0] : NULL);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Q8 USB " << id_ << " has not been activated." << std::endl;
        }
    }

    void Q8Usb::write_analog() {
        if (active_ && num_ao_channels_ > 0) {
            t_error result = hil_write_analog(q8_usb_, &ao_channels_nums_[0], num_ao_channels_, &ao_voltages_[0]);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Either Q8 USB " << id_ << " has not been activated, or no analog output channels were passed to the constructor." << std::endl;
        }
    }

    void Q8Usb::write_digital() {
        if (active_ && num_do_channels_ > 0) {
            t_error result = hil_write_digital(q8_usb_, &do_channels_nums_[0], num_do_channels_, &do_states_[0]);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Either Q8 USB " << id_ << "has not been activated, or no digital output channels were passed to the constructor." << std::endl;
        }
    }

    void Q8Usb::write_all() {
        if (active_) {
            t_error result = hil_write(q8_usb_,
                num_ao_channels_ > 0 ? &ao_channels_nums_[0] : NULL, num_ao_channels_,
                NULL, 0,
                num_do_channels_ > 0 ? &do_channels_nums_[0] : NULL, num_do_channels_,
                NULL, 0,
                num_ao_channels_ > 0 ? &ao_voltages_[0] : NULL,
                NULL,
                num_do_channels_ > 0 ? &do_states_[0] : NULL,
                NULL);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Q8 USB " << id_ << " has not been activated." << std::endl;
        }
    }

    void Q8Usb::reload_watchdog() { // TO DO: CHECK STATUS OF WATCHDOG
        if (active_) {
            t_error result = hil_watchdog_reload(q8_usb_);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Q8 USB " << id_ << " has not been activated." << std::endl;
        }
    }

    void Q8Usb::start_watchdog(double watchdog_timeout) {
        if (active_) {
            t_error result = hil_watchdog_start(q8_usb_, watchdog_timeout);
            if (result < 0)
                print_quarc_error(result);
        }
        else {
            std::cout << "ERROR: Q8 USB " << id_ << " has not been activated." << std::endl;
        }
    }

    void Q8Usb::stop_watchdog() {
        t_error result = hil_watchdog_stop(q8_usb_);
        if (result < 0)
            print_quarc_error(result);
        result = hil_watchdog_clear(q8_usb_);
        if (result < 0)
            print_quarc_error(result);
    }

    void Q8Usb::print_quarc_error(t_error result) {
        TCHAR message[512];
        msg_get_error_message(NULL, result, message, sizeof(message));
        _tprintf(_T("%s (error %d)\n"), message, -result);
    }

    uint_vec Q8Usb::get_q8_velocity_channels(uint_vec enc_channels) {
        uint_vec vel_channels = enc_channels;
        std::transform(vel_channels.begin(), vel_channels.end(), vel_channels.begin(), bind2nd(std::plus<uint>(), 14000));
        return vel_channels;
    }
}