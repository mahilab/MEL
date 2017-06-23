#pragma once
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "util.h"
#include <boost/filesystem.hpp>

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

    // VIRTUAL FUNCTIONS FOR IMPLEMENTING A WATCHDOG TIMER
    
	virtual void reload_watchdog() {}
	virtual void start_watchdog(double watchdog_timeout) {}
	virtual void stop_watchdog() {}

    // FUNCTIONS FOR GETTING/SETTING DAQ STATES

    double_vec get_analog_voltages();
    double get_analog_voltage(int channel_number);
    void set_analog_voltages(double_vec new_voltages);
    void set_analog_voltage(int channel_number, double new_voltage);

    char_vec get_digital_states();
    char get_digital_state(int channel_number);
    void set_digital_states(char_vec new_states);
    void set_digital_state(int channel_number, char new_state);

    int_vec get_encoder_counts();
    int get_encoder_count(int channel_number);
    double_vec get_encoder_count_rates();
    virtual double get_encoder_count_rate(int channel_number);

    // DAQ DATA LOGGING

    std::string   data_log_filename_; // filename of the data log
    std::ofstream data_log_;          // stream for logging to the data log file
    void log_data(double timestamp);  // function to log all state information to data log file

    // STATE VARIABLES

    bool active_ = false;             // bool indicating whether the DAQ is currently active

    double_vec ai_voltages_;          // vector for which analog inputs should be read in to
    double_vec ao_voltages_;          // vector for which analog outputs should be read out from
    char_vec   di_states_;            // vector for which digital inputs should be read in to
    char_vec   do_states_;            // vector for which digital outputs should be read out from
    int_vec    enc_counts_;           // vector for which encoder counts should be read in to
    double_vec enc_counts_per_sec_;   // vector for which encoder counts per second should be read in to

    // CHANNEL NUMBERS BEING USED
                                       
    uint_vec   ai_channels_;          // vector of analog input channels being used 
    uint_vec   ao_channels_;          // vector of analog output channels being used 
    uint_vec   di_channels_;          // vector of digital input channels being used 
    uint_vec   do_channels_;          // vector of digital output channels being used 
    uint_vec   enc_channels_;         // vector of encoder channels being used
    uint_vec   vel_channels_;         // vector of encoder velocity channels being used

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

    int_vec    enc_initial_counts_;   // vector of initial encoder counts to be used when zero_encoder_counts() is called

    // HELPLER FUNCTIONS

    uint channel_number_to_index(const uint_vec& channels, const uint channel_number);
    void sort_and_reduce_channels(uint_vec& channels);
    
};