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

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        Daq(std::string name,
            channel_vec ai_channels,
            channel_vec ao_channels,
            channel_vec di_channels,
            channel_vec do_channels,
            channel_vec encoder_channels,
            channel_vec encrate_channels);

        ////////////////////////////////////
        // GENERIC DAQ MEMBERS AND FUNCTIONS
        ////////////////////////////////////

    public:

        /// Channel struct containing a pointer to a Daq and a channel number.
        struct Channel {
            Daq* daq_;
            channel channel_number_;
            Channel() : daq_(nullptr), channel_number_(-1) {};
            Channel(Daq* daq, channel channel_number) : daq_(daq), channel_number_(channel_number) {}
        };

        const std::string name_;

        /// This function should activate the specific DAQ by opening a communication line, declaring channels, setting ranges, etc.
        virtual int activate() = 0;
        /// This function should deactivate the specific DAQ by running any shutdown procedures and closing the commication line.
        virtual int deactivate() = 0;
        /// This function should read from all DAQ input channels simultenously and save the values to the corresponding data vectors.
        virtual void read_all() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement read_all()" << std::endl; }
        /// This function should write to all DAQ output channels simultaenously from the corresponding data vectors.
        virtual void write_all() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement write_all()" << std::endl; }        
        /// This function should start the watchdog timer if the DAQ supports one.
        virtual void start_watchdog(double watchdog_timeout) {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement start_watchdog()" << std::endl; }
        /// This function should reload the watchdog timer if the DAQ supports one.
        virtual void reload_watchdog() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement reload_watchdog()" << std::endl; }
        /// This function should stop and/or clear the watchdog timer if the DAQ supports one.
        virtual void stop_watchdog() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement stop_watchdog()" << std::endl; }
        /// Logs all current input and output values to the Daq log file when called.
        void log_data(double timestamp);

    protected:

        bool              active_ = false;       ///< bool indicating whether the DAQ is currently active
        const std::string log_dir_ = "daq_logs"; ///< folder where Daq data logs will be stored
        const std::string data_log_filename_;    ///< filename of this Daq data log
        std::ofstream     data_log_;             ///< stream for logging to the Daq data log file

        /// Returns index of a channel number in a channel numbers vector.
        channel_vec::size_type channel_number_to_index(const channel_vec& channels, const channel channel_number);
        /// Sorts and reduces a channel numbers vector such as {3, 1, 1, 2} to {1, 2, 3}.
        channel_vec sort_and_reduce_channels(channel_vec channels);

        /////////////////////////////////////
        // ANALOG INPUT MEMBERS AND FUNCTIONS
        /////////////////////////////////////

    public:

        /// This function should read all analog input channels and save the values to #ai_voltages_ .
        virtual void read_analogs() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement read_analogs()" << std::endl; }
        /// Returns a vector of the most recent analog input vloltages (a copy of #ai_voltages_).
        voltage_vec get_analog_voltages();
        /// Returns the most recent analog input voltage from a given channel number.
        voltage get_analog_voltage(channel channel_number);
        /// This function should set the DAQ's analog input ranges and store the values in #ai_min_voltages_ and #ai_max_voltages_ .
        virtual void set_ai_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) {
            ai_min_voltages_ = min_voltages; ai_max_voltages_ = max_voltages; }

        /// 
        struct Ai : Channel {
            Ai() : Channel() {}
            Ai(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            voltage get_voltage() { return daq_->get_analog_voltage(channel_number_); }
        };

        virtual Ai ai_(channel channel_number) { return Ai(this, channel_number); }

    protected:

        voltage_vec       ai_voltages_;        ///< vector for which analog inputs should be read in to
        const channel_vec ai_channel_nums_;    ///< vector of analog input channels numbers being used 
        const size_t      ai_channels_count_;  ///< vector of analog input channels being used 
        voltage_vec       ai_min_voltages_;    ///< vector of minimum input voltages allowed by the board 
        voltage_vec       ai_max_voltages_;    ///< vector of maximum input voltages allowed by the board 

        //////////////////////////////////////
        // ANALOG OUTPUT MEMBERS AND FUNCTIONS
        //////////////////////////////////////

    public:

        virtual void write_analogs() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement write_analogs()" << std::endl; }
        virtual void set_analog_voltages(voltage_vec new_voltages);
        virtual void set_analog_voltage(channel channel_number, voltage new_voltage);        
        virtual void set_ao_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) { 
            ao_min_voltages_ = min_voltages; ao_max_voltages_ = max_voltages; }
        virtual void set_ao_initial_voltages(voltage_vec initial_voltages) { 
            ao_initial_voltages_ = initial_voltages; }
        virtual void set_ao_final_voltages(voltage_vec final_voltages) { 
            ao_final_voltages_ = final_voltages; }
        virtual void set_ao_expire_voltages(voltage_vec expire_voltages) {
            ao_expire_voltages_ = expire_voltages; }

        struct Ao : Channel {
            Ao() : Channel() {}
            Ao(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            void set_voltage(voltage new_voltage) { daq_->set_analog_voltage(channel_number_, new_voltage); }
        };

        virtual Ao ao_(channel channel_number) { return Ao(this, channel_number); }

    protected:   

        voltage_vec       ao_voltages_;          ///< vector for which analog outputs should be written out from
        const channel_vec ao_channel_nums_;      ///< vector of analog output channels numbers being used 
        const size_t      ao_channels_count_;    ///< vector of analog output channels being used 
        voltage_vec       ao_min_voltages_;      ///< vector of minimum output voltages allowed by the board 
        voltage_vec       ao_max_voltages_;      ///< vector of maximum output voltages allowed by the board 
        voltage_vec       ao_initial_voltages_;  ///< vector of voltages analog outputs will go to when the program starts 
        voltage_vec       ao_final_voltages_;    ///< vector of voltages analog outputs will go to when the program finishes 
        voltage_vec       ao_expire_voltages_;   ///< vector of voltages analog outputs will go to if the watchdog expires 

        //////////////////////////////////////
        // DIGITAL INPUT MEMBERS AND FUNCTIONS
        //////////////////////////////////////

    public:

        virtual void read_digitals() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement read_digitals()" << std::endl; }
        virtual dsignal_vec get_digital_signals();
        virtual dsignal get_digital_signal(channel channel_number);

        struct Di : Channel {
            Di() : Channel() {}
            Di(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            dsignal get_signal() { return daq_->get_digital_signal(channel_number_); }
        };

        virtual Di di_(channel channel_number) { return Di(this, channel_number); }

    protected:

        dsignal_vec       di_signals_;         ///< vector for which digital inputs should be read in to
        const channel_vec di_channel_nums_;    ///< vector of digital input channels numbers being used 
        const size_t      di_channels_count_;  ///< vector of digital input channels being used 

        ////////////////////////////////////////
        // DIGITAL OUTPUT MEMBERS AND FUNCTIONS
        ///////////////////////////////////////

    public:

        virtual void write_digitals() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement write_digitals()" << std::endl; }
        virtual void set_digital_signals(dsignal_vec new_signals);
        virtual void set_digital_signal(channel channel_number, dsignal new_signal);
        virtual void set_do_initial_signals(dsignal_vec initial_signals) { 
            do_initial_signals_ = initial_signals; }
        virtual void set_do_final_signals(dsignal_vec final_signals) { 
            do_final_signals_ = final_signals; }
        virtual void set_do_expire_signals(dsignal_vec expire_signals) { 
            do_expire_signals_ = expire_signals; }

        struct Do : Channel {
            Do() : Channel() {}
            Do(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            void set_signal(dsignal new_signal) { daq_->set_digital_signal(channel_number_, new_signal); }
        };

        virtual Do do_(channel channel_number) { return Do(this, channel_number); }

    protected:

        dsignal_vec do_signals_;               ///< vector for which digital outputs should be written out from
        const channel_vec   do_channel_nums_;  ///< vector of digital output channels numbers being used 
        const size_t   do_channels_count_;     ///< vector of digital output channels being used 
        dsignal_vec do_initial_signals_;       ///< vector of signals digital outputs will go to when the program starts 
        dsignal_vec do_final_signals_;         ///< vector of signals digital outputs will go to when the program finishes 
        dsignal_vec do_expire_signals_;        ///< vector of signals digital outputs will go to if the watchdog expires

        ////////////////////////////////
        // ENCODER MEMBERS AND FUNCTIONS
        ////////////////////////////////

    public:

        virtual void read_encoders() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement read_encoders()" << std::endl; }
        virtual void read_encrates() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement read_encrates()" << std::endl; }
        virtual void zero_encoders() {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement zero_encoders()" << std::endl; }
        virtual void offset_encoders(int32_vec offset_counts) {
            std::cout << "WARNING: DAQ <" << name_ << "> does not implement offset_encoders()" << std::endl; }
        virtual int32_vec get_encoder_counts();
        virtual int32 get_encoder_count(channel channel_number);
        virtual double_vec get_encoder_rates();
        virtual double get_encoder_rate(channel channel_number);
        virtual void set_encoder_quadrature_factors(uint32_vec quadrature_factors) { 
            encoder_quadrature_factors_ = quadrature_factors; }
        uint32_vec get_encoder_quadrature_factors();
        uint32 get_encoder_quadrature_factor(channel channel_number);

        struct Encoder : Channel {
            Encoder() : Channel() {}
            Encoder(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            int32 get_count() { return daq_->get_encoder_count(channel_number_); }
            uint32 get_quadrature_factor() { return daq_->get_encoder_quadrature_factor(channel_number_); }
        };

        struct EncRate : Channel {
            EncRate() : Channel() {}
            EncRate(Daq* daq, channel channel_number) : Channel(daq, channel_number) {}
            double get_rate() { return daq_->get_encoder_rate(channel_number_); }
            uint32 get_quadrature_factor() { return daq_->get_encoder_quadrature_factor(channel_number_); }
        };

        virtual Encoder encoder_(channel channel_number) { return Encoder(this, channel_number); }
        virtual EncRate encrate_(channel channel_number) { return EncRate(this, channel_number); }

    protected:

        int32_vec   enc_counts_;                    ///< vector for which encoder counts should be read in to
        double_vec  enc_rates;                      ///< vector for which encoder rates should be read in to
        const channel_vec   encoder_channel_nums_;  ///< vector of encoder channels being numbers used
        const channel_vec   encrate_channel_nums_;  ///< vector of encoder velocity channels numbers being used
        const size_t   encoder_channels_count_;     ///< vector of encoder channels being used
        const size_t   encrate_channels_count_;     ///< vector of encoder velocity channels being used
        uint32_vec  encoder_quadrature_factors_;    ///< vector of encoder quadrature factors, e.g. 1, 2, or 4
        
    };
}