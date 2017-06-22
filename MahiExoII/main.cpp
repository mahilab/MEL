#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"

// Ctrl-C handler 
static bool  stop = false;                          // flag used to stop the controller
static void signal_handler(int signum) {
	stop = true;
}

int main(int argc, char * argv[]) {

	std::cout << "Have fun, Troy! - Craig" << std::endl;

	// register signal SIGINT and SIGBREAK with signal handler
	signal(SIGINT, signal_handler);
	signal(SIGBREAK, signal_handler);

	// instantiate Q8 USB for encoders, actuator controls, and EMG
    std::string id = "0";
	uint_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
	uint_vec  ao_channels = { 0, 1, 2, 3, 4 };
	uint_vec  di_channels = {};
	uint_vec  do_channels = { 1, 2, 3, 4, 5 };
	uint_vec enc_channels = { 0, 1, 2, 3, 4 };
	char options[] = "update_rate=fast;decimation=1";
	Daq *q8_0 = new Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);
	
	// instantiate Q8 USB for ATI and Futek force/torque sensors
	std::string id = "1";
	uint_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
	uint_vec  ao_channels = {};
	uint_vec  di_channels = {};
	uint_vec  do_channels = {};
	uint_vec enc_channels = {};
	char options[] = "update_rate=fast;decimation=1";
	Daq *q8_1 = new Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

	// initialize Q8 USB
	if (!q8_0->init()) {
		std::cout << "Terminating controller" << std::endl;
		return -1;
	}

	while (!stop) {
		// Do some controls stuffs
	}

	return 0;

}
