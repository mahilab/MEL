#include "Q8Usb.h"
#include "hil.h"
#include "quanser_messages.h"
#include <iostream>
#include <tchar.h>
#include <functional>
#include <string>
#include "Clock.h"
#include <thread>

namespace mel {

    namespace dev {

        Q8Usb::Q8Usb(uint32 id,
            channel_vec ai_channels,
            channel_vec ao_channels,
            channel_vec di_channels,
            channel_vec do_channels,
            channel_vec enc_channels,
            Options options,
            bool sanity_check_on_enable) :
            Daq("q8_usb_" + std::to_string(id),
                ai_channels,
                ao_channels,
                di_channels,
                do_channels,
                enc_channels,
                get_q8_encrate_channels(enc_channels)),
            id_(id),
            options_(options),
            sanity_check_on_enable_(sanity_check_on_enable)
        {
            // set up analog input channels
            for (auto it = ai_channel_nums_.begin(); it != ai_channel_nums_.end(); ++it) {
                ai_min_voltages_.push_back(options_.ai_min_voltages_[*it]);
                ai_max_voltages_.push_back(options_.ai_max_voltages_[*it]);
            }

            // set up analog output channels
            for (auto it = ao_channel_nums_.begin(); it != ao_channel_nums_.end(); ++it) {
                ao_min_voltages_.push_back(options_.ao_min_voltages_[*it]);
                ao_max_voltages_.push_back(options_.ao_max_voltages_[*it]);
                ao_initial_voltages_.push_back(options_.ao_initial_voltages_[*it]);
                ao_final_voltages_.push_back(options_.ao_final_voltages_[*it]);
                ao_expire_voltages_.push_back(options_.ao_expire_voltages_[*it]);
            }

            // set up digital output channels
            for (auto it = do_channel_nums_.begin(); it != do_channel_nums_.end(); ++it) {
                do_initial_signals_.push_back(options_.do_initial_signals_[*it]);
                do_final_signals_.push_back(options_.do_final_signals_[*it]);
                do_expire_signals_.push_back(options_.do_expire_signals_[*it]);
            }

            // set up encoder channels
            for (auto it = encoder_channel_nums_.begin(); it != encoder_channel_nums_.end(); ++it) {
                encoder_quadrature_factors_.push_back(options_.encoder_quadrature_factors_[*it]);
            }

            // build options string
            strcpy(options_str_, options_.build().c_str());
        }

        Q8Usb::~Q8Usb() {
            if (enabled_)
                disable();
        }

        void Q8Usb::enable() {
            if (!enabled_) {
                enabled_ = true;
                t_error result;
                // Attempt to Open Q8 USB and Sanity Check Encoder Velocity Readings (10 attempts)            
                for (int attempt = 0; attempt < 10; attempt++) {
                    std::cout << "Q8 USB " << id_ << ": Enabling (Attempt " << attempt + 1 << ") ... ";
                    result = hil_open("q8_usb", std::to_string(id_).c_str(), &q8_usb_);
                    if (result == 0) {                        
                        if (sanity_check_on_enable_){
                            bool sane = sanity_check();
                            if (sane) {
                                std::cout << "Done" << std::endl;
                                break;
                            }
                            else {
                                std::cout << "Failed (Did Not Pass Sanity Check)" << std::endl;
                                result = 1;
                            }
                        }
                        else {
                            std::cout << "Done" << std::endl;
                            break;
                        }                    
                    }
                    else {
                        std::cout << "Failed" << std::endl;
                        print_quarc_error(result);
                    }
                }

                // If all attempts were unsuccessful, display message and terminate the application.
                if (result != 0) {
                    std::cout << "Q8 USB " << id_ << ": Exhausted all attempts to enable." << std::endl;
                    disable();
                    return;
                }

                // Configure Q8 USB (Functions called in same order as Simulink compiled code)
                std::cout << "Q8 USB " << id_ << ": Configuring ... ";
                result = hil_set_card_specific_options(q8_usb_, options_str_, strlen(options_str_)); // TODO: make this optional or configured in another way
                if (result < 0) {
                    std::cout << "Failed" << std::endl;
                    print_quarc_error(result);
                    return;
                }

                // stop and clear watchdog
                stop_watchdog();
                // set analog I/O ranges
                set_ao_voltage_ranges(ao_min_voltages_, ao_max_voltages_);
                set_ai_voltage_ranges(ai_min_voltages_, ai_max_voltages_);
                // set expiration states
                set_ao_expire_voltages(ao_expire_voltages_);
                set_do_expire_signals(do_expire_signals_);
                // set encoder quadrature factors
                set_encoder_quadrature_factors(encoder_quadrature_factors_);
                // set and write initial voltages and states
                ao_voltages_ = ao_initial_voltages_;
                do_signals_ = do_initial_signals_;
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
                return;
            }
            return;
        }

        void Q8Usb::disable() {
            if (enabled_) {
                t_error result;
                
                // Stop all tasks and monitors (possibly unnecessary)
                hil_task_stop_all(q8_usb_);
                hil_monitor_stop_all(q8_usb_);
                
                // set and write final voltages and states
                ao_voltages_ = ao_final_voltages_;
                do_signals_ = do_final_signals_;
                write_all();

                // Delete all tasks and monitors (possibly unnecessary)
                hil_task_delete_all(q8_usb_);
                hil_monitor_delete_all(q8_usb_);

                // Stop and Clear Watchdog
                stop_watchdog();
                
                // Reset the Q8 board sepcifc options to factory defaults
                factory_reset();
                
                // Wait a few milliseconds so factory_reset() can finish configuring the FPGA
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                // Set and write final voltages and states (one more time)
                ao_voltages_ = ao_final_voltages_;
                do_signals_ = do_final_signals_;
                write_all();

                // Close Q8 USB
                std::cout << "Q8 USB " << id_ << ": Disabling ... ";
                result = hil_close(q8_usb_);
                if (result != 0) {
                    std::cout << "Failed" << std::endl;
                    print_quarc_error(result);
                    return;
                }
                enabled_ = false;
                std::cout << "Done" << std::endl;
                return;
            }
            return;
        }

        bool Q8Usb::sanity_check() {
            read_encrates();
            bool sane = true;
            for (int i = 0; i < enc_rates.size(); ++i) {
                if (enc_rates[i] != 0) {
                    sane = false;
                    break;
                }
            }
            std::fill(enc_rates.begin(), enc_rates.end(), 0.0);
            return sane;
        }

        void Q8Usb::reset() {
            if (enabled_) {
                stop_watchdog();
                ao_voltages_ = ao_initial_voltages_;
                do_signals_ = do_initial_signals_;
                write_all();
            }
        }

        void Q8Usb::factory_reset() {
            Options factory_options;
            factory_options.update_rate_ = Options::UpdateRate::Default;
            char factory_options_str[4096];
            strcpy(factory_options_str, factory_options.build().c_str());
            t_error result = hil_set_card_specific_options(q8_usb_, factory_options_str, strlen(factory_options_str));
            if (result < 0) {
                std::cout << "ERROR: Failed to reset Q8 USB " << id_ << ".";
                print_quarc_error(result);
                return;
            }
        }

        void Q8Usb::zero_encoders() {
            if (enabled_ && encoder_channels_count_ > 0) {
                std::cout << "Q8 USB " << id_ << ": Zeroing encoder counts ... ";
                int32_vec enc_zero_counts(encoder_channels_count_, 0);
                t_error result = hil_set_encoder_counts(q8_usb_, &encoder_channel_nums_[0], static_cast<uint32>(encoder_channels_count_), &enc_zero_counts[0]);
                if (result != 0) {
                    std::cout << "Failed" << std::endl;
                    print_quarc_error(result);
                    return;
                }
                std::cout << "Done" << std::endl;
            }
        }


        void Q8Usb::offset_encoders(int32_vec offset_counts) {
            if (enabled_ && encoder_channels_count_ > 0) {
                std::cout << "Q8 USB " << id_ << ": Offsetting encoder counts ... ";
                offset_counts.resize(encoder_channels_count_, 0);
                t_error result = hil_set_encoder_counts(q8_usb_, &encoder_channel_nums_[0], static_cast<uint32>(encoder_channels_count_), &offset_counts[0]);
                if (result != 0) {
                    std::cout << "Failed" << std::endl;
                    print_quarc_error(result);
                    return;
                }
                std::cout << "Done" << std::endl;
            }
        }

        void Q8Usb::read_analogs() {
            if (enabled_ && ai_channels_count_ > 0) {
                t_error result = hil_read_analog(q8_usb_, &ai_channel_nums_[0], static_cast<uint32>(ai_channels_count_), &ai_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Either Q8 USB " << id_ << " has not been enabled, or no analog input channels were passed to the constructor." << std::endl;
            }
        }

        void Q8Usb::read_digitals() {
            if (enabled_ && di_channels_count_ > 0) {
                t_error result = hil_read_digital(q8_usb_, &di_channel_nums_[0], static_cast<uint32>(di_channels_count_), &di_signals_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Either Q8 USB " << id_ << " has not been enabled, or no digital input channels were passed to the constructor." << std::endl;
            }
        }

        void Q8Usb::read_encoders() {
            if (enabled_ && encoder_channels_count_ > 0) {
                t_error result = hil_read_encoder(q8_usb_, &encoder_channel_nums_[0], static_cast<uint32>(encoder_channels_count_), &enc_counts_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Either Q8 USB " << id_ << " has not been enabled, or no encoder channels were passed to the constructor." << std::endl;
            }
        }

        void Q8Usb::read_encrates() {
            if (enabled_ && encrate_channels_count_ > 0) {
                t_error result = hil_read_other(q8_usb_, &encrate_channel_nums_[0], static_cast<uint32>(encrate_channels_count_), &enc_rates[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Either Q8 USB " << id_ << "has not been enabled, or no encoder channels were passed to the constructor." << std::endl;
            }
        }

        void Q8Usb::read_all() {
            if (enabled_) {
                t_error result = hil_read(q8_usb_,
                    ai_channels_count_ > 0 ? &ai_channel_nums_[0] : NULL, static_cast<uint32>(ai_channels_count_),
                    encoder_channels_count_ > 0 ? &encoder_channel_nums_[0] : NULL, static_cast<uint32>(encoder_channels_count_),
                    di_channels_count_ > 0 ? &di_channel_nums_[0] : NULL, static_cast<uint32>(di_channels_count_),
                    encrate_channels_count_ > 0 ? &encrate_channel_nums_[0] : NULL, static_cast<uint32>(encrate_channels_count_),
                    ai_channels_count_ > 0 ? &ai_voltages_[0] : NULL,
                    encoder_channels_count_ > 0 ? &enc_counts_[0] : NULL,
                    di_channels_count_ > 0 ? &di_signals_[0] : NULL,
                    encrate_channels_count_ > 0 ? &enc_rates[0] : NULL);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Q8 USB " << id_ << " has not been enabled." << std::endl;
            }
        }

        void Q8Usb::write_analogs() {
            if (enabled_ && ao_channels_count_ > 0) {
                t_error result = hil_write_analog(q8_usb_, &ao_channel_nums_[0], static_cast<uint32>(ao_channels_count_), &ao_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Either Q8 USB " << id_ << " has not been enabled, or no analog output channels were passed to the constructor." << std::endl;
            }
        }

        void Q8Usb::write_digitals() {
            if (enabled_ && do_channels_count_ > 0) {
                t_error result = hil_write_digital(q8_usb_, &do_channel_nums_[0], static_cast<uint32>(do_channels_count_), &do_signals_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Either Q8 USB " << id_ << "has not been enabled, or no digital output channels were passed to the constructor." << std::endl;
            }
        }

        void Q8Usb::write_all() {
            if (enabled_) {
                t_error result = hil_write(q8_usb_,
                    ao_channels_count_ > 0 ? &ao_channel_nums_[0] : NULL, static_cast<uint32>(ao_channels_count_),
                    NULL, 0,
                    do_channels_count_ > 0 ? &do_channel_nums_[0] : NULL, static_cast<uint32>(do_channels_count_),
                    NULL, 0,
                    ao_channels_count_ > 0 ? &ao_voltages_[0] : NULL,
                    NULL,
                    do_channels_count_ > 0 ? &do_signals_[0] : NULL,
                    NULL);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Q8 USB " << id_ << " has not been enabled." << std::endl;
            }
        }

        void Q8Usb::start_watchdog(double watchdog_timeout) {
            if (enabled_) {
                t_error result = hil_watchdog_start(q8_usb_, watchdog_timeout);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Q8 USB " << id_ << " has not been enabled." << std::endl;
            }
        }

        void Q8Usb::reload_watchdog() { // TO DO: CHECK STATUS OF WATCHDOG
            if (enabled_) {
                t_error result = hil_watchdog_reload(q8_usb_);
                if (result < 0)
                    print_quarc_error(result);
            }
            else {
                std::cout << "ERROR: Q8 USB " << id_ << " has not been enabled." << std::endl;
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

        bool Q8Usb::is_watchdog_expired() {
            t_error result = hil_watchdog_is_expired(q8_usb_);
            if (result == 1)
                return true;
            else if (result == 0)
                return false;
            else {
                print_quarc_error(result);
                return false;
            }
        }

        void Q8Usb::benchmark(uint32 samples) {

            if (enabled_) {

                util::print("Benchmarking Q8 USB " + std::to_string(id_) + " with " + std::to_string(samples) + " samples.");

                double start, stop;

                // analog input benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    read_analogs();
                stop = util::Clock::global_time();
                util::print("AI: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

                // analog output benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    write_analogs();
                stop = util::Clock::global_time();
                util::print("AO: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

                // digital input benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    read_digitals();
                stop = util::Clock::global_time();
                util::print("DI: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

                // digital output benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    write_digitals();
                stop = util::Clock::global_time();
                util::print("DO: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

                // encoder benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    read_encoders();
                stop = util::Clock::global_time();
                util::print("ENCODER: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

                // encrate benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    read_encrates();
                stop = util::Clock::global_time();
                util::print("ENCRATE: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

                // read all benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    read_all();
                stop = util::Clock::global_time();
                util::print("READ ALL: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

                // write all benchmark
                start = util::Clock::global_time();
                for (uint32 i = 0; i < samples; i++)
                    write_all();
                stop = util::Clock::global_time();
                util::print("WRITE ALL: " + std::to_string(stop - start) + "s (" + std::to_string((int)(samples / (util::Clock::global_time() - start))) + " Hz)");

            }
            else {
                std::cout << "ERROR: Q8 USB " << id_ << " has not been enabled." << std::endl;
            }
        }

        void Q8Usb::set_ai_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) {
            Daq::set_ai_voltage_ranges(min_voltages, max_voltages);
            if (ai_channels_count_ > 0) {
                t_error result = hil_set_analog_input_ranges(q8_usb_, &ai_channel_nums_[0], static_cast<uint32>(ai_channels_count_), &ai_min_voltages_[0], &ai_max_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
        }

        void Q8Usb::set_ao_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) {
            Daq::set_ao_voltage_ranges(min_voltages, max_voltages);
            if (ao_channels_count_ > 0) {
                t_error result = hil_set_analog_output_ranges(q8_usb_, &ao_channel_nums_[0], static_cast<uint32>(ao_channels_count_), &ao_min_voltages_[0], &ao_max_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
        }

        void Q8Usb::set_ao_expire_voltages(voltage_vec expire_voltages) {
            Daq::set_ao_expire_voltages(expire_voltages);
            if (ao_channels_count_ > 0) {
                t_error result = hil_watchdog_set_analog_expiration_state(q8_usb_, &ao_channel_nums_[0], static_cast<uint32>(ao_channels_count_), &ao_expire_voltages_[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
        }

        void Q8Usb::set_do_expire_signals(dsignal_vec expire_signals) {
            Daq::set_do_expire_signals(expire_signals);
            if (do_channels_count_ > 0) {
                // must convert MEL type to Quanser type
                std::vector<t_digital_state> converted_do_exp_signals;
                for (auto it = do_expire_signals_.begin(); it != do_expire_signals_.end(); ++it) {
                    if (*it == 1) {
                        converted_do_exp_signals.push_back(DIGITAL_STATE_HIGH);
                    }
                    else {
                        converted_do_exp_signals.push_back(DIGITAL_STATE_LOW);
                    }
                }
                t_error result = hil_watchdog_set_digital_expiration_state(q8_usb_, &do_channel_nums_[0], static_cast<uint32>(do_channels_count_), &converted_do_exp_signals[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
        }

        void Q8Usb::set_encoder_quadrature_factors(uint32_vec quadrature_factors) {
            Daq::set_encoder_quadrature_factors(quadrature_factors);
            if (encoder_channels_count_ > 0) {
                // must convert MEL type to Quanser type
                std::vector<t_encoder_quadrature_mode> converted_encoder_modes;
                for (auto it = encoder_quadrature_factors_.begin(); it != encoder_quadrature_factors_.end(); ++it) {
                    if (*it == 1)
                        converted_encoder_modes.push_back(ENCODER_QUADRATURE_NONE);
                    else
                        converted_encoder_modes.push_back(ENCODER_QUADRATURE_4X);
                }
                t_error result = hil_set_encoder_quadrature_mode(q8_usb_, &encoder_channel_nums_[0], static_cast<uint32>(encoder_channels_count_), &converted_encoder_modes[0]);
                if (result < 0)
                    print_quarc_error(result);
            }
        }

        void Q8Usb::print_quarc_error(int result) {
            TCHAR message[512];
            msg_get_error_message(NULL, result, message, sizeof(message));
            _tprintf(_T("QUARC ERROR: %s (error %d)\n"), message, -result);
        }

        channel_vec Q8Usb::get_q8_encrate_channels(channel_vec enc_channels) {
            uint32_vec vel_channels = enc_channels;
            std::transform(vel_channels.begin(), vel_channels.end(), vel_channels.begin(), bind2nd(std::plus<uint32>(), 14000));
            return vel_channels;
        }


        std::string Q8Usb::Options::build() {
            std::string options = "";

            // append update rate
            if (update_rate_ == UpdateRate::Normal_1kHz || update_rate_ == UpdateRate::Default)
                options += "update_rate=normal;";
            else if (update_rate_ == UpdateRate::Fast_8kHz)
                options += "update_rate=fast;";

            // append decimation
            options += "decimation=" + std::to_string(decimation_) + ";";

            // append iterative options
            for (int i = 0; i < 8; i++) {

                std::string enc = "enc" + std::to_string(i) + "_";
                if (enc_dir_[i] == EncDir::Reversed)
                    options += enc + "dir=1;";
                if (enc_filter_[i] == EncFilter::Filtered)
                    options += enc + "filter=1;";
                if (enc_a_[i] == EncDetection::Low)
                    options += enc + "a=1;";
                if (enc_b_[i] == EncDetection::Low)
                    options += enc + "b=1;";
                if (enc_z_[i] == EncDetection::Low)
                    options += enc + "z=1;";
                if (enc_reload_[i] == EncReload::OnPulse)
                    options += enc + "reload=1;";
                if (enc_velocity_[i] > 0.0) {
                    std::string velocity = std::to_string(enc_velocity_[i]); velocity.resize(7);
                    options += enc + "velocity=" + velocity + ";";
                }

                std::string ch = "ch" + std::to_string(i) + "_";
                if (ao_modes_[i].mode_ != AoMode::Mode::Default) {
                    std::string kff = std::to_string(ao_modes_[i].kff_);    kff.resize(7);
                    std::string a0 = std::to_string(ao_modes_[i].a0_);     a0.resize(7);
                    std::string a1 = std::to_string(ao_modes_[i].a1_);     a1.resize(7);
                    std::string a2 = std::to_string(ao_modes_[i].a2_);     a2.resize(7);
                    std::string b0 = std::to_string(ao_modes_[i].b0_);     b0.resize(7);
                    std::string b1 = std::to_string(ao_modes_[i].b1_);     b1.resize(7);
                    std::string post = std::to_string(ao_modes_[i].post_);   post.resize(7);

                    options += ch + "mode=" + std::to_string(ao_modes_[i].mode_) + ";";
                    options += ch + "kff=" + kff + ";";
                    options += ch + "a0=" + a0 + ";";
                    options += ch + "a1=" + a1 + ";";
                    options += ch + "a2=" + a2 + ";";
                    options += ch + "b0=" + b0 + ";";
                    options += ch + "b1=" + b1 + ";";
                    options += ch + "post=" + post + ";";
                }
            }
            return options;
        }

        int Q8Usb::get_q8_usb_count() {
            int id = 0;
            std::vector<t_card> q8_usbs;
            t_error result;
            while (true) {
                t_card q8_usb;
                result = hil_open("q8_usb", std::to_string(id).c_str(), &q8_usb);
                if (result < 0) {
                    break;
                }
                else {
                    q8_usbs.push_back(q8_usb);
                    ++id;
                }
            }
            for (size_t i = 0; i < q8_usbs.size(); ++i) {
                hil_close(q8_usbs[i]);
            }
            return id;
        }

        bool Q8Usb::check_digital_loopback(uint32 daq_id, channel digital_channel) {
            std::cout << "Q8 USB: Checking Digital Loopback for Channel " << digital_channel << " ... " << std::endl;

            //  create a Q8Usb object
            uint32 id = daq_id;
            channel_vec  ai_channels = {};
            channel_vec  ao_channels = {};
            channel_vec  di_channels = { digital_channel };
            channel_vec  do_channels = { digital_channel };
            channel_vec enc_channels = {};
            core::Daq* q8_temp = new Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels);

            // create clock
            util::Clock clock(100);

            dsignal di_signal = 0;
            dsignal do_signal = 0;
            bool is_connected = true;

            q8_temp->enable();
            q8_temp->start_watchdog(0.1);

            clock.start();

            for (auto i = 0; i < 11; i++) {

                q8_temp->reload_watchdog();

                if (i > 0) {
                    q8_temp->read_digitals();
                    di_signal = q8_temp->get_digital_signal(digital_channel);
                    if (di_signal != do_signal) {
                        is_connected = false;
                    }
                    do_signal = 1 - do_signal;
                }

                q8_temp->set_digital_signal(digital_channel, do_signal);
                q8_temp->write_digitals();

                clock.wait();
            }

            delete q8_temp;
            q8_temp = nullptr;

            if (is_connected) {
                std::cout << "Q8 USB: Channel " << digital_channel << " Connected" << std::endl;
            }
            else {
                std::cout << "Q8 USB: Channel " << digital_channel << " Failed" << std::endl;
            }
            
            return is_connected;
        }
    }
}
