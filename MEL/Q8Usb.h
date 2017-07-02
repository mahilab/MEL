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

        // DEFAULT Q8 USB CHANNEL SETTINGS

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

        // inhereted virtual functions from Daq class to be overriden
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

        virtual void set_ai_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) override;
        virtual void set_ao_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) override;

        virtual void set_ao_expire_voltages(voltage_vec expire_voltages) override;
        virtual void set_do_expire_signals(dsignal_vec expire_signals) override;

        virtual void set_encoder_quadrature_factors(uint32_vec quadrature_factors) override;


    private:

        // HELPTER FUNCTIONS 

        static void print_quarc_error(t_error result);
        static channel_vec get_q8_encrate_channels(channel_vec enc_channels);

    public:

        // Q8 USB BOARD SPECIFIC OPTIONS (WIP)
        // http://www.quanser.com/Products/quarc/documentation/q8_usb.html

        class Options {

        public:

            enum class UpdateRate { Default = 0, Normal_1kHz = 1, Fast_8kHz = 2 };
            enum class EncoderDirection { Default = 0, Reversed = 1 };
            enum class EncoderFilter { Default = 0, Filtered = 1 };
            enum class EncoderDetection { Default = 0, High = 0, Low = 1 };
            enum class EncoderReload { Default = 0, OnPulse = 1 };

            struct AnalogOutputMode {

                enum Mode { Default = 0, VoltageMode = 0, CurrentMode2 = 1, CurrentMode1 = 2, CurrentMode0 = 3, ControlMode2 = 4, ControlMode1 = 5, ControlMode0 = 6 };
                
                Mode mode = Mode::Default;
                double kff_  = 1;
                double a0_   = 1;
                double a1_   = 1;
                double a2_   = 1;
                double b0_   = 1;
                double b1_   = 1;
                double post_ = 1;

            };

            uint32 decimation_ = 1;
            UpdateRate update_rate_ = UpdateRate::Fast_8kHz;
            std::vector<EncoderDirection> enc_dir_ = std::vector<EncoderDirection>(8, EncoderDirection::Default);
            std::vector<EncoderFilter> enc_filter_ = std::vector<EncoderFilter>(8, EncoderFilter::Default);
            std::vector<EncoderDetection> enc_a_ = std::vector<EncoderDetection>(8, EncoderDetection::Default);
            std::vector<EncoderDetection> enc_b_ = std::vector<EncoderDetection>(8, EncoderDetection::Default);
            std::vector<EncoderDetection> enc_z_ = std::vector<EncoderDetection>(8, EncoderDetection::Default);
            std::vector<EncoderReload> enc_reload_ = std::vector<EncoderReload>(8, EncoderReload::Default);
            std::vector<double> enc_velocity_ = std::vector<double>(8, 0.0);
            std::vector<AnalogOutputMode> ao_modes_ = std::vector<AnalogOutputMode>(8, AnalogOutputMode());

        private:

            friend class Q8Usb;

            std::string build();

        };

    };
}
