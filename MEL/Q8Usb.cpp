#include "Q8Usb.h"

const double Q8Usb::ai_min_voltage_ = -10;
const double Q8Usb::ai_max_voltage_ = +10;
const double Q8Usb::ao_min_voltage_ = -10;
const double Q8Usb::ao_max_voltage_ = +10;
const double Q8Usb::ao_initial_voltage_ = 0;
const double Q8Usb::ao_final_voltage_ = 0;
const double Q8Usb::ao_exp_voltage_ = 0;
const char   Q8Usb::do_initial_state_ = 0;
const char   Q8Usb::do_final_state_ = 0;
const t_digital_state Q8Usb::do_exp_state_ = DIGITAL_STATE_TRISTATE;
const int   Q8Usb::enc_initial_count_ = 0;
const t_encoder_quadrature_mode Q8Usb::enc_mode_ = ENCODER_QUADRATURE_4X;

Q8Usb::Q8Usb(std::string id,
    uint_vec ai_channels,
	uint_vec ao_channels,
	uint_vec di_channels,
	uint_vec do_channels,
	uint_vec enc_channels,
	char * options) :
    Daq("q8_usb", id, 
        ai_channels, 
        ao_channels, 
        di_channels, 
        do_channels, 
        enc_channels,
        get_q8_velocity_channels(enc_channels))
{    
	/* set up analog input channels */
	ai_min_voltages_ = double_vec(ai_channels_.size(), ai_min_voltage_);
	ai_max_voltages_ = double_vec(ai_channels_.size(), ai_max_voltage_);

	/* set up analog output channels */
	ao_min_voltages_ = double_vec(ao_channels_.size(), ao_min_voltage_);
	ao_max_voltages_ = double_vec(ao_channels_.size(), ao_max_voltage_);
	ao_initial_voltages_ = double_vec(ao_channels_.size(), ao_initial_voltage_);
	ao_final_voltages_ = double_vec(ao_channels_.size(), ao_final_voltage_);
	ao_exp_voltages_ = double_vec(ao_channels_.size(), ao_exp_voltage_);

	/* set up digital output channels */
	do_initial_states_ = char_vec(do_channels_.size(), do_initial_state_);
	do_final_states_ = char_vec(do_channels_.size(), do_final_state_);
	do_exp_states_ = std::vector<t_digital_state>(do_channels_.size(), do_exp_state_);

	/* set up encoder channels */
	enc_modes_ = std::vector<t_encoder_quadrature_mode>(enc_channels.size(), enc_mode_);
	enc_initial_counts_ = int_vec(enc_channels.size(), 0);

	/* set up options */
	strcpy(options_, options);

    /* TODO: Make this a function of num channels passed in */
    data_log_ << "Time" << "\t"
        << "ai0" << "\t"
        << "ai1" << "\t"
        << "ai2" << "\t"
        << "ao0" << "\t"
        << "ao1" << "\t"
        << "ao2" << "\t"
        << "di0" << "\t"
        << "di1" << "\t"
        << "di2" << "\t"
        << "do0" << "\t"
        << "do1" << "\t"
        << "do2" << "\t"
        << "enc0" << "\t"
        << "enc1" << "\t"
        << "enc2" << "\t"
        << "vel0" << "\t"
        << "vel1" << "\t"
        << "vel2" << std::endl;
}

int Q8Usb::init() {
	if (!active_) {
		t_error result;
		// Attempt to Open Q8 USB and Sanity Check Encoder Velocity Readings (10 attempts)
		std::cout << "Opening Q8 USB ... ";
		for (int attempt = 0; attempt < 10; attempt++) {
			result = hil_open("q8_usb", id_.c_str(), &q8_usb_);
			if (result == 0) {
				double temp[3];
				result = hil_read_other(q8_usb_, &vel_channels_[0], enc_channels_.size(), temp);
				if (temp[0] == 0 && temp[1] == 0 && temp[2] == 0) {
					// std::cout << "Attempt " << attempt + 1 << ": Success" << std::endl;
					std::cout << "Done" << std::endl;
					break;
				}
				else {
					// std::cout << "Attempt " << attempt + 1 << ": Encoder Reading Errors" << std::endl;
					result = 1;
					hil_close(q8_usb_);
				}
			}
			else {
				// std::cout << "Attempt " << attempt + 1 << ": Failed to Open" << std::endl;
			}
		}

		// If all attempts were unsuccessful, display message and terminate the application.
		if (result != 0) {
			std::cout << "Failed" << std::endl;
			return 0;
		}

		// Configure Q8 USB (Functions called in same order as Simulink compiled code)
		std::cout << "Configuring Q8 USB ... ";
		result = hil_set_card_specific_options(q8_usb_, options_, strlen(options_));
		if (result < 0)
			print_quarc_error(result);

		// Stop and Clear Watchdog
		result = hil_watchdog_stop(q8_usb_);
		if (result < 0)
			print_quarc_error(result);
		result = hil_watchdog_clear(q8_usb_);
		if (result < 0)
			print_quarc_error(result);
		result = hil_set_analog_input_ranges(q8_usb_, &ai_channels_[0], ai_channels_.size(), &ai_min_voltages_[0], &ai_max_voltages_[0]);
		if (result < 0)
			print_quarc_error(result);
		result = hil_set_analog_output_ranges(q8_usb_, &ao_channels_[0], ao_channels_.size(), &ao_min_voltages_[0], &ao_max_voltages_[0]);
		if (result < 0)
			print_quarc_error(result);
		result = hil_write_analog(q8_usb_, &ao_channels_[0], ao_channels_.size(), &ao_initial_voltages_[0]);
		if (result != 0)
			print_quarc_error(result);
		result = hil_watchdog_set_analog_expiration_state(q8_usb_, &ao_channels_[0], ao_channels_.size(), &ao_exp_voltages_[0]);
		if (result < 0)
			print_quarc_error(result);

		// hil_set_digital_directions
		result = hil_write_digital(q8_usb_, &do_channels_[0], do_channels_.size(), &do_initial_states_[0]);
		if (result != 0)
			print_quarc_error(result);
		// Set Digital Expiration States
		result = hil_watchdog_set_digital_expiration_state(q8_usb_, &do_channels_[0], do_channels_.size(), &do_exp_states_[0]);
		if (result < 0)
			print_quarc_error(result);

		// Set Encoder Quadrature Mode
		result = hil_set_encoder_quadrature_mode(q8_usb_, &enc_channels_[0], enc_channels_.size(), &enc_modes_[0]);
		if (result < 0)
			print_quarc_error(result);

		// TO BE IMPLEMENTED IN FUTURE (IN THIS ORDER):

		// hil_set_pwm_mode
		// hil_set_pwm_frequency
		// hil_set_pwm_duty_cycle
		// hil_set_pwm_configuration
		// hil_set_pwm_deadband
		// hil_write_pwm
		// hil_watchdog_set_pwm_expiration_state

		active_ = true;
		std::cout << "Done" << std::endl;
		return 1;
	}
}

void Q8Usb::zero_encoder_counts() {
	if (active_) {
		std::cout << "Zeroing Encoder Counts ... ";
		t_error result = hil_set_encoder_counts(q8_usb_, &enc_channels_[0], enc_channels_.size(), &enc_initial_counts_[0]);
		if (result != 0)
			print_quarc_error(result);
		std::cout << "Done" << std::endl;
	}
}

int Q8Usb::terminate() {
	if (active_) {
		t_error result;

		// Stop all tasks and monitors (possibly unnecessary)
		hil_task_stop_all(q8_usb_);
		hil_monitor_stop_all(q8_usb_);

		// Set Final Output Values
		hil_write_analog(q8_usb_, &ao_channels_[0], ao_channels_.size(), &ao_final_voltages_[0]);
		hil_write_digital(q8_usb_, &do_channels_[0], do_channels_.size(), &do_final_states_[0]);

		// Delete all tasks and monitors (possibly unnecessary)
		hil_task_delete_all(q8_usb_);
		hil_monitor_delete_all(q8_usb_);

		// Stop and Clear Watchdog
		hil_watchdog_stop(q8_usb_);
		hil_watchdog_clear(q8_usb_);

		// Close Q8 USB
		std::cout << "Closing Q8 USB ... ";
		result = hil_close(q8_usb_);
		if (result != 0) {
			std::cout << "Failed" << std::endl;
			print_quarc_error(result);
			return 0;
		}
		active_ = false;
		std::cout << "Done" << std::endl;
		return 1;
	}
	return 0;
}

void Q8Usb::read_analog() {
	if (active_) {
		t_error result = hil_read_analog(q8_usb_, &ai_channels_[0], ai_channels_.size(), &ai_voltages_[0]);
		if (result < 0)
			print_quarc_error(result);
	}
}

void Q8Usb::read_digital() {
    if (active_) {
        t_error result = hil_read_digital(q8_usb_, &di_channels_[0], di_channels_.size(), &di_states_[0]);
        if (result < 0)
            print_quarc_error(result);
    }
}

void Q8Usb::read_encoder() {
    if (active_) {
        t_error result = hil_read_encoder(q8_usb_, &enc_channels_[0], enc_channels_.size(), &enc_counts_[0]);
        if (result < 0)
            print_quarc_error(result);
    }
}

void Q8Usb::read_encoder_velocity() {
    if (active_) {
        t_error result = hil_read_other(q8_usb_, &vel_channels_[0], enc_channels_.size(), &enc_counts_per_sec_[0]);
        if (result < 0)
            print_quarc_error(result);
    }
}

void Q8Usb::read_all() {
    if (active_) {
        t_error result = hil_read(q8_usb_,
            &ai_channels_[0], ai_channels_.size(),
            &enc_channels_[0], enc_channels_.size(),
            &di_channels_[0], di_channels_.size(),
            &vel_channels_[0], enc_channels_.size(),
            &ai_voltages_[0],
            &enc_counts_[0],
            &di_states_[0],
            &enc_counts_per_sec_[0]);
        if (result < 0)
            print_quarc_error(result);
    }
}

void Q8Usb::write_analog() {
	if (active_) {
		t_error result = hil_write_analog(q8_usb_, &ao_channels_[0], ao_channels_.size(), &ao_voltages_[0]);
		if (result < 0)
			print_quarc_error(result);
	}
}

void Q8Usb::write_digital() {
	if (active_) {
		t_error result = hil_write_digital(q8_usb_, &do_channels_[0], do_channels_.size(), &do_states_[0]);
		if (result < 0)
			print_quarc_error(result);
	}
}

void Q8Usb::reload_watchdog() {
	if (active_) {
		t_error result = hil_watchdog_reload(q8_usb_);
		if (result < 0)
			print_quarc_error(result);
	}
}

void Q8Usb::start_watchdog(double watchdog_timeout) {
	if (active_) {
		t_error result = hil_watchdog_start(q8_usb_, watchdog_timeout);
		if (result < 0)
			print_quarc_error(result);
	}
}

void Q8Usb::stop_watchdog() {
	hil_watchdog_stop(q8_usb_);
	hil_watchdog_clear(q8_usb_);
}

void Q8Usb::log_data(double timestamp) {
    /* TODO: this should probably open and close the data_log_ each time */
    
    data_log_ << timestamp << "\t"
        << ai_voltages_[0] << "\t"
        << ai_voltages_[1] << "\t"
        << ai_voltages_[2] << "\t"
        << ao_voltages_[0] << "\t"
        << ao_voltages_[1] << "\t"
        << ao_voltages_[2] << "\t"
        << (int)di_states_[0] << "\t"
        << (int)di_states_[1] << "\t"
        << (int)di_states_[2] << "\t"
        << (int)do_states_[0] << "\t"
        << (int)do_states_[1] << "\t"
        << (int)do_states_[2] << "\t"
        << enc_counts_[0] << "\t"
        << enc_counts_[1] << "\t"
        << enc_counts_[2] << "\t"
        << enc_counts_per_sec_[0] << "\t"
        << enc_counts_per_sec_[1] << "\t"
        << enc_counts_per_sec_[2] << std::endl;
}

void Q8Usb::print_quarc_error(t_error result) {
    TCHAR message[512];
    msg_get_error_message(NULL, result, message, sizeof(message));
    _tprintf(_T("%s (error %d)\n"), message, -result);
}

uint_vec Q8Usb::get_q8_velocity_channels(uint_vec enc_channels) {
    uint_vec vel_channels = enc_channels;
    std::transform(vel_channels.begin(), vel_channels.end(), vel_channels.begin(), bind2nd(std::plus<uint>(), 14000));
    return vel_channels;
}