#pragma once
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include "Daq.h"
#include <thread>
#include <chrono>
#include "hil.h"
#include "quanser_messages.h"

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

    private:

        // DEFAULT Q8 USB SETTINGS

        const voltage  default_ai_min_voltage_     = -10;
        const voltage  default_ai_max_voltage_     = +10;
        const voltage  default_ao_min_voltage_     = -10;
        const voltage  default_ao_max_voltage_     = +10;
        const voltage  default_ao_initial_voltage_ = 0;
        const voltage  default_ao_final_voltage_   = 0;
        const voltage  default_ao_exp_voltage_     = 0;
        const dsignal  default_do_initial_signal_  = 0;
        const dsignal  default_do_final_signal_    = 0;
        const dsignal  default_do_expire_signal_   = 0;
        const uint32   default_encoder_quadrature_factor_ = 4; 

        // QUARC SPECIFIC TYPES AND OPTIONS

        t_card q8_usb_;
        char       options_[1024];   // Quarc board specific options

    public:

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

        // HELPTER FUNCTIONS 

        static void print_quarc_error(t_error result);
        static channel_vec get_q8_encrate_channels(channel_vec enc_channels);

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
