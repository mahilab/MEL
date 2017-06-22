#pragma once
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include "hil.h"
#include "quanser_messages.h"
#include "Daq.h"
#include <thread>
#include <chrono>

class Q8Usb : public Daq {

public:

	/* constructor */
    Q8Usb(std::string name,
        uint_vec ai_channels,
		uint_vec ao_channels,
		uint_vec di_channels,
		uint_vec do_channels,
		uint_vec enc_channels,
		char * options);

	/* inhereted virtual functions from Daq class to be implemented */
	int init();
	int terminate();

	void zero_encoder_counts();

	void read_analog();
	void read_digital();
    void read_encoder();
    void read_encoder_velocity();
    void read_all();

	void write_digital();
    void write_analog();

	void reload_watchdog();
	void start_watchdog(double watchdog_timeout);
	void stop_watchdog();

    void log_data(double timestamp);

private:

	t_card q8_usb_;
	std::vector<t_digital_state> do_exp_states_;
	std::vector<t_encoder_quadrature_mode> enc_modes_;

	char       options_[1024];   /* board specific options which include current controller gains */

	/* private static members */
	static const double  ai_min_voltage_;
	static const double  ai_max_voltage_;

	static const double  ao_min_voltage_;
	static const double  ao_max_voltage_;
	static const double  ao_initial_voltage_;
	static const double  ao_final_voltage_;
	static const double  ao_exp_voltage_;

	static const char    do_initial_state_;
	static const char    do_final_state_;
	static const t_digital_state do_exp_state_;

	static const int     enc_initial_count_;
	static const t_encoder_quadrature_mode enc_mode_;

	/* private functions */
	static void print_quarc_error(t_error result);
    static uint_vec get_q8_velocity_channels(uint_vec enc_channels);
};

