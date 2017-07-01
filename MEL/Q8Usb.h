#pragma once
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include "hil.h"
#include "quanser_messages.h"
#include "Daq.h"
#include <thread>
#include <chrono>

namespace mel {

    class Q8Usb : public Daq {

    public:

        /* constructor */
        Q8Usb(std::string name,
            channel_vec ai_channels,
            channel_vec ao_channels,
            channel_vec di_channels,
            channel_vec do_channels,
            channel_vec enc_channels,
            char * options);

        /* inhereted virtual functions from Daq class to be implemented */
        int activate() override;
        int deactivate() override;

        void zero_encoders() override;
        void offset_encoders(int32_vec offset_counts) override;

        void read_analogs() override;
        void read_digitals() override;
        void read_encoders() override;
        void read_encrates() override;
        void read_all() override;

        void write_digitals() override;
        void write_analogs() override;
        void write_all() override;

        void reload_watchdog() override;
        void start_watchdog(double watchdog_timeout) override;
        void stop_watchdog() override;

        double get_encoder_rate(channel channel_number) override {
            channel_number += 14000;
            return Daq::get_encoder_rate(channel_number);
        }

    private:

        t_card q8_usb_;
        std::vector<t_digital_state> do_exp_signals_;
        std::vector<t_encoder_quadrature_mode> enc_modes_;

        char       options_[1024];   /* board specific options which include current controller gains */

        // Q8 USB SETTINGS (WITH DEFAULTS INITIALIZED)

        voltage  ai_min_voltage_ = -10;
        voltage  ai_max_voltage_ = +10;
        voltage  ao_min_voltage_ = -10;
        voltage  ao_max_voltage_ = +10;
        voltage  ao_initial_voltage_ = 0;
        voltage  ao_final_voltage_ = 0;
        voltage  ao_exp_voltage_ = 0;
        dsignal   do_initial_signal_ = 0;
        dsignal   do_final_signal_ = 0;

        t_digital_state do_exp_state_ = DIGITAL_STATE_LOW;
        t_encoder_quadrature_mode enc_mode_ = ENCODER_QUADRATURE_4X;

        // HELPTER FUNCTIONS 

        static void print_quarc_error(t_error result);
        static channel_vec get_q8_velocity_channels(channel_vec enc_channels);

    public:

        // Q8 USB BOARD SPECIFIC OPTIONS (W.I.P.)

        enum class Q8EncoderDirction { Default, Normal, Reversed };
        std::vector<Q8EncoderDirction> encoder_directions_;

        enum class Q8EncoderFilterMode { Default, Unfiltered, Filtered };
        std::vector<Q8EncoderFilterMode> encoder_filter_modes_;

        enum class Q8EncoderDetectionMode { Default, Low, High };
        std::vector<Q8EncoderDetectionMode> encoder_A_modes_;
        std::vector<Q8EncoderDetectionMode> encoder_B_modes_;
        std::vector<Q8EncoderDetectionMode> encoder_Z_modes_;

        enum class Q8EncoderReloadMode { Default, Manual, OnPulse };
        std::vector<Q8EncoderReloadMode> encoder_reload_modes_;

        enum class Q8UpdateRate { Default, Normal_1kHz, Fast_8kHz };
        Q8UpdateRate update_rate_;

        enum class Q8AnalogOutputMode { Default, VoltageMode, CurrentMode0, CurrentMode1, CurrentMode2, ControlMode0, ControlMode1, ControlMode2 };
        std::vector<Q8AnalogOutputMode> ao_modes_;


    };
}
