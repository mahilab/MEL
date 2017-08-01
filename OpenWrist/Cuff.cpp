#include "Cuff.h"


int Cuff::enable() {
	
	int result = 0;

	std::cout << "Initializing CUFF ...";

	short int currents[2];
	short int realmotpos[2];
	
	/* attempt to open port*/
	for (int attempt = 0; attempt < 10; attempt++) {
		int opened = open_port();
		if (opened)
			break;
		else if (attempt == 9) {
			std::cout << "Failed. Could not open port." << std::endl;
			return 0;
		}
	}

	/* attempt to activate communications */
	char activated = 0;
	for (int attempt = 0; attempt < 10; attempt++) {
		commActivate(&comm_settings_t_, CUFF_ID, 1);
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		commGetActivate(&comm_settings_t_, CUFF_ID, &activated);
		if (activated)
			break;
		else if (attempt == 9){
			std::cout << "Failed. Could not activate communications." << std::endl;
			return 0;
		}
	}

	/* set initial motor positions */
	short int motpos_zero[2];
	reference_motor_positions_[0] = 0;
	reference_motor_positions_[1] = 0;
	commSetInputs(&comm_settings_t_, CUFF_ID, motpos_zero);

	/* start IO thread */
	io_loop_ = true;
    io_thread_ = boost::thread(&Cuff::io_thread_func,this);
	std::cout << "Done" << std::endl;
	return 1;
}

int Cuff::disable() {
    set_motor_positions(0, 0);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    return 1;
}

int Cuff::open_port() {
	FILE *file;
	char port[255] = { 'C','O','M','6' };

	openRS485(&comm_settings_t_, port, 2000000);

	if (comm_settings_t_.file_handle == INVALID_HANDLE_VALUE)
	{
		puts("Couldn't connect to the serial port.");
		return 0;
	}
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	return 1;
}

// Function to select between all the ports
int Cuff::port_selection() {
    int i;
    int aux_int;
    int num_ports = 0;
    char my_port[255];
    char ports[10][255];
    FILE *file;

    while (1) {
        num_ports = RS485listPorts(ports);

        if (num_ports) {
            puts("\nChoose the serial port for your QB:\n");

            for (i = 0; i < num_ports; ++i) {
                printf("[%d] - %s\n\n", i + 1, ports[i]);
            }
            printf("Serial port: ");
            scanf("%d", &aux_int);
            getchar();

            if (aux_int && (aux_int <= num_ports)) {
                strcpy(my_port, ports[aux_int - 1]);
            }
            else {
                puts("Choice not available");
                continue;
            }

            file = fopen(QBMOVE_FILE, "w+");
            if (file == NULL) {
                printf("Cannot open qbmove.conf\n");
            }
            fprintf(file, "serialport %s\n", my_port);
            fclose(file);
            return 1;

        }
        else {
            puts("No serial port available.");
            return 0;
        }
    }
}

void Cuff::pretensioning(short int* motpos) {
	struct timespec req, elaptime;
	boost::chrono::system_clock::time_point tstart, tend;
	boost::chrono::nanoseconds elapsed_time;
	int elapsed_time_ns;
	int tv_usec, tv_sec;
	short int currents[2];
	short int stepmot;

	motpos[0] = 0;
	motpos[1] = 0;

	tstart = boost::chrono::system_clock::now();

	for (int i = 0; i < 80; i++) {

		commGetCurrents(&comm_settings_t_, CUFF_ID, currents);

		motpos[0] = motpos[0] - (10 * (50 - currents[0]));
		motpos[1] = motpos[1] + (10 * (50 + currents[1]));
		std::cout << currents[0] << '\t' << currents[1] << '\t' << motpos[0] << '\t' << motpos[1] << '\n';

		commSetInputs(&comm_settings_t_, CUFF_ID, motpos);

		tend = boost::chrono::system_clock::now();
		elapsed_time = boost::chrono::system_clock::now() - tstart;

		elapsed_time_ns = ((double)elapsed_time.count());

		if (elapsed_time_ns > 50000) {
			int nsec = (int)((((elapsed_time_ns) / 1000.0) + 50000.0) / 1000000.0) + 1;
			elapsed_time_ns -= 1000000000 * nsec;
			tv_sec = nsec;
		}
		if (((double)elapsed_time.count()) / 1000.0 < 1050000) {
			int nsec = (int)(-(((elapsed_time_ns) / 1000.0) + 50000.0) / 1000000.0);
			elapsed_time_ns += 1000000000 * nsec;
			tv_sec -= nsec;
		}

		elapsed_time_ns = elapsed_time_ns + tv_sec - (elapsed_time_ns - 50000) * 1000;

		boost::this_thread::sleep(boost::posix_time::milliseconds(elapsed_time_ns / 1000000));
		tstart = boost::chrono::system_clock::now();
	}

	boost::this_thread::sleep(boost::posix_time::milliseconds(25));

	stepmot = 0;
	tstart = boost::chrono::system_clock::now();
	for (int i = 0; i < 80; i++) {

		stepmot = stepmot + 15;
		if (stepmot < 1200) {

			motpos[0] = motpos[0] + 15;
			motpos[1] = motpos[1] - 15;

			commSetInputs(&comm_settings_t_, CUFF_ID, motpos);
		}

		tend = boost::chrono::system_clock::now();
		elapsed_time = boost::chrono::system_clock::now() - tstart;

		elapsed_time_ns = ((double)elapsed_time.count());

		if (elapsed_time_ns > 50000) {
			int nsec = (int)((((elapsed_time_ns) / 1000.0) + 50000.0) / 1000000.0) + 1;
			elapsed_time_ns -= 1000000000 * nsec;
			tv_sec = nsec;
		}
		if (((double)elapsed_time.count()) / 1000.0 < 1050000) {
			int nsec = (int)(-(((elapsed_time_ns) / 1000.0) + 50000.0) / 1000000.0);
			elapsed_time_ns += 1000000000 * nsec;
			tv_sec -= nsec;
		}

		elapsed_time_ns = elapsed_time_ns + tv_sec - (elapsed_time_ns - 50000) * 1000;

		boost::this_thread::sleep(boost::posix_time::milliseconds(elapsed_time_ns / 1000000));
		tstart = boost::chrono::system_clock::now();
	}
}

int Cuff::io_thread_func() {
	while (io_loop_) {
		// boost::mutex::scoped_lock lock(io_mutex_);
		commSetInputs(&comm_settings_t_, CUFF_ID, reference_motor_positions_);
		commGetMeasurements(&comm_settings_t_, CUFF_ID, actual_motor_positions_);
		commGetCurrents(&comm_settings_t_, CUFF_ID, actual_motor_currents_);
		//std::cout << "Mot0: " << actual_motor_positions_[0] << ",   Mot1: " << actual_motor_positions_[1] << ",   Ref0: " << reference_motor_positions_[0] << ",   Ref1:" << reference_motor_positions_[1] << std::endl;
	}
    return 1;
}

void Cuff::set_motor_positions(short int motor_position_0, short int motor_position_1) {
    reference_motor_positions_[0] = motor_position_0;
    reference_motor_positions_[1] = motor_position_1;
}