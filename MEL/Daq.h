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
            uint_vec enc_channels,
            uint_vec vel_channels);

        std::string type_;       // string representing the DAQ type e.g. "q8_usb"
        std::string id_;         // string representing the DAQ ID number e.g. "0"

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

        std::string   data_log_filename_; // filename of the data log
        std::ofstream data_log_;          // stream for logging to the data log file
        void log_data(double timestamp);  // function to log all state information to data log file

    protected:

        // STATE VARIABLES

        bool active_ = false;             // bool indicating whether the DAQ is currently active

        double_vec ai_voltages_;          // vector for which analog inputs should be read in to
        double_vec ao_voltages_;          // vector for which analog outputs should be read out from
        char_vec   di_states_;            // vector for which digital inputs should be read in to
        char_vec   do_states_;            // vector for which digital outputs should be read out from
        int_vec    enc_counts_;           // vector for which encoder counts should be read in to
        double_vec enc_rates;             // vector for which encoder counts per second should be read in to

        // CHANNEL NUMBERS BEING USED

        uint_vec   ai_channels_;          // vector of analog input channels being used 
        uint_vec   ao_channels_;          // vector of analog output channels being used 
        uint_vec   di_channels_;          // vector of digital input channels being used 
        uint_vec   do_channels_;          // vector of digital output channels being used 
        uint_vec   encoder_channels_;     // vector of encoder channels being used
        uint_vec   encrate_channels;      // vector of encoder velocity channels being used

        // NUMBER OF CHANNELS DEFINED

        uint   num_ai_channels_;          // vector of analog input channels being used 
        uint   num_ao_channels_;          // vector of analog output channels being used 
        uint   num_di_channels_;          // vector of digital input channels being used 
        uint   num_do_channels_;          // vector of digital output channels being used 
        uint   num_enc_channels_;         // vector of encoder channels being used
        uint   num_vel_channels_;         // vector of encoder velocity channels being used

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

        // HELPLER FUNCTIONS

        uint channel_number_to_index(const uint_vec& channels, const uint channel_number);  // returns index of a channel number in the channels vector
        void sort_and_reduce_channels(uint_vec& channels);                                  // turns input such as {3, 1, 1, 2} to {1, 2, 3}      

    public:

        // CHANNEL STRUCTS AND SETS

        struct Channel { 
            Daq* daq_; 
            uint channel_; 
            Channel(Daq* daq, uint channel) : daq_(daq), channel_(channel) {} 
        };

        struct AiChannel : Channel { 
            AiChannel(Daq* daq, uint channel) : Channel(daq, channel) {} 
            double get_voltage() { return daq_->get_analog_voltage(channel_); }
        };

        struct AoChannel : Channel {
            AoChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            void set_voltage(double new_voltage) { daq_->set_analog_voltage(channel_, new_voltage ); }
        };

        struct DiChannel : Channel {
            DiChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            char get_state() { return daq_->get_digital_state(channel_); }
        };

        struct DoChannel : Channel {
            DoChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            void set_state(double new_state) { daq_->set_digital_state(channel_, new_state); }
        };

        struct EncoderChannel : Channel {
            EncoderChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            int get_count() { return daq_->get_encoder_count(channel_); }
        };

        struct EncRateChannel : Channel {
            EncRateChannel(Daq* daq, uint channel) : Channel(daq, channel) {}
            double get_rate() { return daq_->get_encoder_rate(channel_); }
        };


        struct AoDoChannelSet {
            Daq* daq_;
            uint ao_channel_;
            uint do_channel_;
            AoDoChannelSet(Daq* daq, uint ao_channel, uint do_channel) : daq_(daq), ao_channel_(ao_channel), do_channel_(do_channel) {};
        };

        struct FullChannelSet {
            Daq* daq_;
            uint ai_channel_;
            uint ao_channel_;
            uint di_channel_;
            uint do_channel_;
            uint encoder_channel_;
            uint encrate_channel_;
            FullChannelSet(Daq* daq, uint ai_channel, uint ao_channel,
                           uint di_channel, uint do_channel,
                           uint encoder_channel, uint encrate_channel) :
                daq_(daq), 
                ai_channel_(ai_channel), ao_channel_(ao_channel),
                di_channel_(di_channel), do_channel_(do_channel),
                encoder_channel_(encoder_channel), encrate_channel_(encrate_channel)
            {};
        };

        // FUNCTIONS FOR GETTING CHANNEL STRUCTS AND SETS

        virtual AiChannel ai_channel(uint channel_number) { return AiChannel(this, channel_number); }
        virtual AoChannel ao_channel(uint channel_number) { return AoChannel(this, channel_number); }
        virtual DiChannel di_channel(uint channel_number) { return DiChannel(this, channel_number); }
        virtual DoChannel do_channel(uint channel_number) { return DoChannel(this, channel_number); }
        virtual EncoderChannel encoder_channel(uint channel_number) { return EncoderChannel(this, channel_number); }
        virtual EncRateChannel encrate_channel(uint channel_number) { return EncRateChannel(this, channel_number); }

        virtual AoDoChannelSet ao_do_channel_set(uint ao_channel, uint do_channel) {
            return AoDoChannelSet(this, ao_channel, do_channel);
        }
        virtual FullChannelSet full_channel_set(uint ai_channel, uint ao_channel, uint di_channel, uint do_channel, uint encoder_channel, uint encrate_channel) {
            return FullChannelSet(this, ai_channel, ao_channel, di_channel, do_channel, encoder_channel, encrate_channel);
        }
        
    };
}