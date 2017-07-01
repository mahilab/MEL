#pragma once
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "util.h"
#include <boost/filesystem.hpp>

namespace mel {

    class Daq {

    public:

        // CONSTRUCTOR / DESTRUCTOR 

        Daq(std::string name, std::string id,
            channel_vec ai_channels,
            channel_vec ao_channels,
            channel_vec di_channels,
            channel_vec do_channels,
            channel_vec encoder_channels,
            channel_vec encrate_channels);

        const std::string type_;       // string representing the DAQ type e.g. "q8_usb"
        const std::string id_;         // string representing the DAQ ID number e.g. "0"

        // PURE VIRTUAL FUNCTIONS FOR ACTIVATING AND DEACTIVATING DAQ (REQ'D BY ALL DERIVED CLASSES)

        virtual int activate() = 0;
        virtual int deactivate() = 0;

        // VIRTUAL FUNCTIONS FOR IMPLEMENTING READING / WRITING FUNCTIONALITY

        virtual void read_analogs()   { std::cout << "WARNING: This DAQ does not implement read_analogs()"   << std::endl; }
        virtual void read_digitals()  { std::cout << "WARNING: This DAQ does not implement read_digitals()"  << std::endl; }
        virtual void read_encoders()  { std::cout << "WARNING: This DAQ does not implement read_encoders()"  << std::endl; }
        virtual void read_encrates()  { std::cout << "WARNING: This DAQ does not implement read_encrates()"  << std::endl; }
        virtual void read_all()       { std::cout << "WARNING: This DAQ does not implement read_all()"       << std::endl; }

        virtual void write_analogs()  { std::cout << "WARNING: This DAQ does not implement write_analogs()"  << std::endl; }
        virtual void write_digitals() { std::cout << "WARNING: This DAQ does not implement write_digitals()" << std::endl; }
        virtual void write_all()      { std::cout << "WARNING: This DAQ does not implement write_all()"      << std::endl;}

        virtual void zero_encoders() {}
        virtual void offset_encoders(int32_vec offset_counts) {}

        // VIRTUAL FUNCTIONS FOR IMPLEMENTING A WATCHDOG TIMER

        virtual void reload_watchdog() {}
        virtual void start_watchdog(double watchdog_timeout) {}
        virtual void stop_watchdog() {}

        // FUNCTIONS FOR GETTING/SETTING DAQ STATES (DEFAULT BEHAVIOR MAY BE OVERRIDDEN IF DESIRED)

        virtual voltage_vec get_analog_voltages();
        virtual voltage get_analog_voltage(channel channel_number);
        virtual void set_analog_voltages(voltage_vec new_voltages);
        virtual void set_analog_voltage(channel channel_number, voltage new_voltage);

        virtual dsignal_vec get_digital_signals();
        virtual dsignal get_digital_signal(channel channel_number);
        virtual void set_digital_signals(dsignal_vec new_signals);
        virtual void set_digital_signal(channel channel_number, dsignal new_signal);

        virtual int32_vec get_encoder_counts();
        virtual int32 get_encoder_count(channel channel_number);
        virtual double_vec get_encoder_rates();
        virtual double get_encoder_rate(channel channel_number);

        // FUNCTIONS FOR SETTING OPTIONS

        virtual void set_ai_max_voltages(voltage_vec max_voltages);
        virtual void set_ai_min_voltages(voltage_vec min_voltages);

        virtual void set_ao_max_voltages(voltage_vec max_voltages);
        virtual void set_ao_min_voltages(voltage_vec min_voltages);
        virtual void set_ao_initial_voltages(voltage_vec initial_voltages);
        virtual void set_ao_final_voltages(voltage_vec final_voltages);
        
        virtual void set_do_initial_states(dsignal_vec initial_states);
        virtual void set_do_final_states(dsignal_vec final_states);

        virtual void set_encoder_quadrature_factors(uint8_vec quadrature_factors);

        // DAQ DATA LOGGING

        const std::string   log_dir_ = "daq_logs"; // folder where data logs will be stored
        const std::string   data_log_filename_;    // filename of the data log
        std::ofstream data_log_;             // stream for logging to the data log file
        void log_data(double timestamp);     // function to log all state information to data log file    

        // STATE VARIABLES

        bool active_ = false;             // bool indicating whether the DAQ is currently active

        voltage_vec ai_voltages_;          // vector for which analog inputs should be read in to
        voltage_vec ao_voltages_;          // vector for which analog outputs should be read out from
        dsignal_vec   di_signals_;            // vector for which digital inputs should be read in to
        dsignal_vec   do_signals_;            // vector for which digital outputs should be read out from
        int32_vec    enc_counts_;           // vector for which encoder counts should be read in to
        double_vec   enc_rates;             // vector for which encoder counts per second should be read in to

        // CHANNEL NUMBERS BEING USED

        const channel_vec   ai_channels_nums_;          // vector of analog input channels numbers being used 
        const channel_vec   ao_channels_nums_;          // vector of analog output channels numbers being used 
        const channel_vec   di_channels_nums_;          // vector of digital input channels numbers being used 
        const channel_vec   do_channels_nums_;          // vector of digital output channels numbers being used 
        const channel_vec   encoder_channels_nums_;     // vector of encoder channels being numbers used
        const channel_vec   encrate_channels_nums_;     // vector of encoder velocity channels numbers being used

        // NUMBER OF CHANNELS DEFINED

        const size_t   num_ai_channels_;          // vector of analog input channels being used 
        const size_t   num_ao_channels_;          // vector of analog output channels being used 
        const size_t   num_di_channels_;          // vector of digital input channels being used 
        const size_t   num_do_channels_;          // vector of digital output channels being used 
        const size_t   num_enc_channels_;         // vector of encoder channels being used
        const size_t   num_vel_channels_;         // vector of encoder velocity channels being used

        // DAQ SETTINGS (TO BE IMPLEMENTED IN DERIVED DAQ CLASSES)

        voltage_vec ai_min_voltages_;      // vector of minimum input voltages allowed by the board 
        voltage_vec ai_max_voltages_;      // vector of maximum input voltages allowed by the board 

        voltage_vec ao_min_voltages_;      // vector of minimum output voltages allowed by the board 
        voltage_vec ao_max_voltages_;      // vector of maximum output voltages allowed by the board 
        voltage_vec ao_initial_voltages_;  // vector of voltages analog outputs will go to when the program starts 
        voltage_vec ao_final_voltages_;    // vector of voltages analog outputs will go to when the program finishes 
        voltage_vec ao_exp_voltages_;      // vector of voltages analog outputs will go to if the watchdog expires 

        dsignal_vec   do_initial_signals_;    // vector of states digital outputs will go to when the program starts 
        dsignal_vec   do_final_signals_;      // vector of states digital outputs will go to when the program finishes 

        uint8_vec   encoder_quadrature_factors_; // vector of encoder quadrature factors, e.g. 1X, 2X, 4X

        // HELPLER FUNCTIONS

        channel_vec::size_type channel_number_to_index(const channel_vec& channels, const channel channel_number);  // returns index of a channel number in the channels vector
        channel_vec sort_and_reduce_channels(channel_vec channels); // turns channel number input such as {3, 1, 1, 2} to {1, 2, 3}      

    public:

        // CHANNEL STRUCTS AND SETS

        struct Channel { 
            Daq* daq_; 
            channel channel_number_; 
            Channel() : daq_(nullptr), channel_number_(-1) {};
            Channel(Daq* daq, channel channel_number) : daq_(daq), channel_number_(channel_number) {}
        };

        struct AiChannel : Channel { 
            AiChannel() : Channel() {}
            AiChannel(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            voltage get_voltage() { return daq_->get_analog_voltage(channel_number_); }
        };

        struct AoChannel : Channel {
            AoChannel() : Channel() {}
            AoChannel(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            void set_voltage(voltage new_voltage) { daq_->set_analog_voltage(channel_number_, new_voltage ); }
        };

        struct DiChannel : Channel {
            DiChannel() : Channel() {}
            DiChannel(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            dsignal get_signal() { return daq_->get_digital_signal(channel_number_); }
        };

        struct DoChannel : Channel {
            DoChannel() : Channel() {}
            DoChannel(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            void set_signal(dsignal new_signal) { daq_->set_digital_signal(channel_number_, new_signal); }
        };

        struct EncoderChannel : Channel {
            EncoderChannel() : Channel() {}
            EncoderChannel(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            int32 get_count() { return daq_->get_encoder_count(channel_number_); }
        };

        struct EncRateChannel : Channel {
            EncRateChannel() : Channel() {}
            EncRateChannel(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            double get_rate() { return daq_->get_encoder_rate(channel_number_); }
        };

        // FUNCTIONS FOR GETTING CHANNEL STRUCTS AND SETS

        virtual AiChannel ai_channel(channel channel_number) { return AiChannel(this, channel_number); }
        virtual AoChannel ao_channel(channel channel_number) { return AoChannel(this, channel_number); }
        virtual DiChannel di_channel(channel channel_number) { return DiChannel(this, channel_number); }
        virtual DoChannel do_channel(channel channel_number) { return DoChannel(this, channel_number); }
        virtual EncoderChannel encoder_channel(channel channel_number) { return EncoderChannel(this, channel_number); }
        virtual EncRateChannel encrate_channel(channel channel_number) { return EncRateChannel(this, channel_number); }
        
    };
}