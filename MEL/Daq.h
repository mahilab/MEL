#pragma once
#include "globals.h"

class Daq {

public:

	/* state variables */
	double_vec ai_voltages_;          /* vector for which analog inputs should be read in to*/
	double_vec ao_voltages_;          /* vector for which analog outputs should be read out from*/
	char_vec   di_states_;            /* vector for which digital inputs should be read in to*/
	char_vec   do_states_;            /* vector for which digital outputs should be read out from*/
	int_vec    enc_counts_;           /* vector for which encoder counts should be read in to*/
	double_vec enc_counts_per_sec_;   /* vector for which encoder counts per second should be read in to*/

									  /* I/O channel information */
	uint_vec   ai_channels_;          /* vector of analog input channels being used */
	double_vec ai_min_voltages_;      /* vector of minimum input voltages allowed by the board */
	double_vec ai_max_voltages_;      /* vector of maximum input voltages allowed by the board */

	uint_vec   ao_channels_;          /* vector of analog output channels being used */
	double_vec ao_min_voltages_;      /* vector of minimum output voltages allowed by the board */
	double_vec ao_max_voltages_;      /* vector of maximum output voltages allowed by the board */
	double_vec ao_initial_voltages_;  /* vector of voltages analog outputs will go to when the program starts */
	double_vec ao_final_voltages_;    /* vector of voltages analog outputs will go to when the program finishes */
	double_vec ao_exp_voltages_;      /* vector of voltages analog outputs will go to if the watchdog expires */

	uint_vec   di_channels_;          /* vector of digital input channels being used */

	uint_vec   do_channels_;          /* vector of digital output channels being used */
	char_vec   do_initial_states_;    /* vector of states digital outputs will go to when the program starts */
	char_vec   do_final_states_;      /* vector of states digital outputs will go to when the program finishes */

	uint_vec   enc_channels_;         /* vector of encoder channels being used */
	uint_vec   vel_channels_;         /* vector of encoder velocity channels being used */
	int_vec    enc_initial_counts_;   /* vector of initial encoder counts to be used when zero_encoder_counts() is called */

									  /* pure virtual functions that must be implemented by all derived DAQs */
	virtual int init() = 0;
	virtual int terminate() = 0;

	virtual void read_analog() = 0;
	virtual void write_analog(double_vec ao_voltages) = 0;

	virtual void read_digital() = 0;
	virtual void write_digital(char_vec do_voltages) = 0;

	virtual void read_encoder() = 0;
	virtual void read_encoder_velocity() = 0;
	virtual void zero_encoder_counts() = 0;

	/* virtual functions that may or may not be implemented by all derived DAQs */
	virtual void reload_watchdog() {}
	virtual void start_watchdog(double watchdog_timeout) {}
	virtual void stop_watchdog() {}

protected:

	bool initialized_ = false;

};