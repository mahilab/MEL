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
            uint_vec ai_channels,
            uint_vec ao_channels,
            uint_vec di_channels,
            uint_vec do_channels,
            uint_vec encoder_channels,
            uint_vec encrate_channels);



        const std::string type_;       // string representing the DAQ type e.g. "q8_usb"
        const std::string id_;         // string representing the DAQ ID number e.g. "0"

        // PURE VIRTUAL FUNCTIONS FOR ACTIVATING AND DEACTIVATING DAQ (REQ'D BY ALL DERIVED CLASSES)

        virtual int activate() = 0;
        virtual int deactivate() = 0;

        // VIRTUAL FUNCTIONS FOR IMPLEMENTING READING / WRITING FUNCTIONALITY

        virtual void read_analog() {}
        virtual void read_digital() {}
        virtual void read_encoder_counts() {}
        virtual void read_encoder_count_rates() {}
        virtual void read_encoder_pulses() {}
        virtual void read_all() {}

        virtual void write_analog() {}
        virtual void write_digital() {}
        virtual void write_all() {}

        virtual void zero_encoder_counts() {}
        virtual void offset_encoder_counts(int_vec offset_counts) {}

        // VIRTUAL FUNCTIONS FOR IMPLEMENTING A WATCHDOG TIMER

        virtual void reload_watchdog() {}
        virtual void start_watchdog(double watchdog_timeout) {}
        virtual void stop_watchdog() {}

        // FUNCTIONS FOR GETTING/SETTING DAQ STATES (DEFAULT BEHAVIOR MAY BE OVERRIDDEN IF DESIRED)

        virtual double_vec get_analog_voltages();
        virtual double get_analog_voltage(int channel_number);
        virtual void set_analog_voltages(double_vec new_voltages);
        virtual void set_analog_voltage(int channel_number, double new_voltage);

        virtual char_vec get_digital_states();
        virtual char get_digital_state(int channel_number);
        virtual void set_digital_states(char_vec new_states);
        virtual void set_digital_state(int channel_number, char new_state);

        virtual int_vec get_encoder_counts();
        virtual int get_encoder_count(int channel_number);
        virtual double_vec get_encoder_rates();
        virtual double get_encoder_rate(int channel_number);

        // DAQ DATA LOGGING

        const std::string   log_dir_ = "daq_logs"; // folder where data logs will be stored
        const std::string   data_log_filename_;    // filename of the data log
        std::ofstream data_log_;             // stream for logging to the data log file
        void log_data(double timestamp);     // function to log all state information to data log file

    

        // STATE VARIABLES

        bool active_ = false;             // bool indicating whether the DAQ is currently active

        double_vec ai_voltages_;          // vector for which analog inputs should be read in to
        double_vec ao_voltages_;          // vector for which analog outputs should be read out from
        char_vec   di_states_;            // vector for which digital inputs should be read in to
        char_vec   do_states_;            // vector for which digital outputs should be read out from
        int_vec    enc_counts_;           // vector for which encoder counts should be read in to
        double_vec enc_rates;             // vector for which encoder counts per second should be read in to

        // CHANNEL NUMBERS BEING USED

        const uint_vec   ai_channels_nums_;          // vector of analog input channels numbers being used 
        const uint_vec   ao_channels_nums_;          // vector of analog output channels numbers being used 
        const uint_vec   di_channels_nums_;          // vector of digital input channels numbers being used 
        const uint_vec   do_channels_nums_;          // vector of digital output channels numbers being used 
        const uint_vec   encoder_channels_nums_;     // vector of encoder channels being numbers used
        const uint_vec   encrate_channels_nums_;     // vector of encoder velocity channels numbers being used

        // NUMBER OF CHANNELS DEFINED

        const uint   num_ai_channels_;          // vector of analog input channels being used 
        const uint   num_ao_channels_;          // vector of analog output channels being used 
        const uint   num_di_channels_;          // vector of digital input channels being used 
        const uint   num_do_channels_;          // vector of digital output channels being used 
        const uint   num_enc_channels_;         // vector of encoder channels being used
        const uint   num_vel_channels_;         // vector of encoder velocity channels being used

        // DAQ SETTINGS (TO BE IMPLEMENTED IN DERIVED DAQ CLASSES)

        double_vec ai_min_voltages_;      // vector of minimum input voltages allowed by the board 
        double_vec ai_max_voltages_;      // vector of maximum input voltages allowed by the board 

        double_vec ao_min_voltages_;      // vector of minimum output voltages allowed by the board 
        double_vec ao_max_voltages_;      // vector of maximum output voltages allowed by the board 
        double_vec ao_initial_voltages_;  // vector of voltages analog outputs will go to when the program starts 
        double_vec ao_final_voltages_;    // vector of voltages analog outputs will go to when the program finishes 
        double_vec ao_exp_voltages_;      // vector of voltages analog outputs will go to if the watchdog expires 

        char_vec   do_initial_states_;    // vector of states digital outputs will go to when the program starts 
        char_vec   do_final_states_;      // vector of states digital outputs will go to when the program finishes 

        uint_vec   encoder_quadrature_factors_; // vector of encoder quadrature factors, e.g. 1X, 2X, 4X

        // HELPLER FUNCTIONS

        uint channel_number_to_index(const uint_vec& channels, const uint channel_number);  // returns index of a channel number in the channels vector
        uint_vec sort_and_reduce_channels(uint_vec channels);                               // turns input such as {3, 1, 1, 2} to {1, 2, 3}      

    public:

        // CHANNEL STRUCTS AND SETS

        struct Channel { 
            Daq* daq_; 
            uint channel_; 
            Channel() : daq_(NULL), channel_(-1) {};
            Channel(Daq* daq, uint channel) : daq_(daq), channel_(channel) {} 
        };

        struct AiChannel : Channel { 
            AiChannel() : Channel() {}
            AiChannel(Daq* daq, uint channel) : Channel(daq, channel) {} 
            double get_voltage() { return daq_->get_analog_voltage(channel_); }
        };

        struct AoChannel : Channel {
            AoChannel() : Channel() {}
            AoChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            void set_voltage(double new_voltage) { daq_->set_analog_voltage(channel_, new_voltage ); }
        };

        struct DiChannel : Channel {
            DiChannel() : Channel() {}
            DiChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            char get_state() { return daq_->get_digital_state(channel_); }
        };

        struct DoChannel : Channel {
            DoChannel() : Channel() {}
            DoChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            void set_state(double new_state) { daq_->set_digital_state(channel_, new_state); }
        };

        struct EncoderChannel : Channel {
            EncoderChannel() : Channel() {}
            EncoderChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            int quadrature_factor_;
            int get_count() { return daq_->get_encoder_count(channel_); }

        };

        struct EncRateChannel : Channel {
            EncRateChannel() : Channel() {}
            EncRateChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            double get_rate() { return daq_->get_encoder_rate(channel_); }
        };

        // FUNCTIONS FOR GETTING CHANNEL STRUCTS AND SETS

        virtual AiChannel ai_channel(uint channel_number) { return AiChannel(this, channel_number); }
        virtual AoChannel ao_channel(uint channel_number) { return AoChannel(this, channel_number); }
        virtual DiChannel di_channel(uint channel_number) { return DiChannel(this, channel_number); }
        virtual DoChannel do_channel(uint channel_number) { return DoChannel(this, channel_number); }
        virtual EncoderChannel encoder_channel(uint channel_number) { return EncoderChannel(this, channel_number); }
        virtual EncRateChannel encrate_channel(uint channel_number) { return EncRateChannel(this, channel_number); }
        
    };
}