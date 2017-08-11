#include "Cuff.h"
#include <chrono>
#include <thread>

int Cuff::enable() {
	
	int result = 0;

	std::cout << "Enabling CUFF ...";

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

	// set initial motor positions
	short int motpos_zero[2];
	reference_motor_positions_[0] = 0;
	reference_motor_positions_[1] = 0;
	commSetInputs(&comm_settings_t_, CUFF_ID, motpos_zero);

	// start IO thread 
	io_loop_ = true;
    io_thread_ = boost::thread(&Cuff::io_thread_func,this);
	std::cout << "Done" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return 1;
}

int Cuff::disable() {
    std::cout << "Disabling CUFF ... ";
    set_motor_positions(0, 0, false);
    commActivate(&comm_settings_t_, CUFF_ID, 0);
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    io_loop_ = false;
    std::cout << "Done" << std::endl;
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

void Cuff::pretensioning(int force_newtons, short int* motpos_zero, short int* scaling_factor) {
    std::cout << "Pretensioning CUFF ... ";
    short int stepmot, act_mot_pos_0, act_mot_pos_1, act_mot_cur_0, act_mot_cur_1;
	std::chrono::high_resolution_clock::time_point tstart, tend;
    std::chrono::nanoseconds elapsed_time;
	int elapsed_time_us;
    int MAX_VAL_CAR_0 = 570;
    int MAX_VAL_CAR_1 = 570;

	motpos_zero[0] = 0;
	motpos_zero[1] = 0;

	tstart = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 80; i++) {
        get_motor_currents(act_mot_cur_0, act_mot_cur_1);
		motpos_zero[0] = motpos_zero[0] - (10 * (50 - act_mot_cur_0));
		motpos_zero[1] = motpos_zero[1] + (10 * (50 + act_mot_cur_1));
        //std::cout << act_mot_cur_0 << '\t' << act_mot_cur_1 << '\t' << motpos_zero[0] << '\t' << motpos_zero[1] << '\n';
        set_motor_positions(motpos_zero[0], motpos_zero[1], false);
		tend = std::chrono::high_resolution_clock::now();
		elapsed_time = std::chrono::high_resolution_clock::now() - tstart;
		elapsed_time_us = (int)((double)elapsed_time.count())/1000;
		std::this_thread::sleep_for(std::chrono::microseconds (50000 - elapsed_time_us));
		tstart = std::chrono::high_resolution_clock::now();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(25));

	stepmot = 0;
	tstart = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 80; i++) {
		stepmot = stepmot + 15;
		if (stepmot < 1200) {
			motpos_zero[0] = motpos_zero[0] + 15;
			motpos_zero[1] = motpos_zero[1] - 15;
            set_motor_positions(motpos_zero[0], motpos_zero[1], false);
		}
        tend = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::high_resolution_clock::now() - tstart;
        elapsed_time_us = (int)((double)elapsed_time.count()) / 1000;
        std::this_thread::sleep_for(std::chrono::microseconds(50000 - elapsed_time_us));
        tstart = std::chrono::high_resolution_clock::now();
	}

    short int motpos[2];
    motpos[0] = motpos_zero[0] - 20000;
    motpos[1] = motpos_zero[1] + 20000;
    set_motor_positions(motpos[0], motpos[1], false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    get_motor_currents(act_mot_pos_0, act_mot_pos_1);
    scaling_factor[0] = abs((act_mot_pos_0 - motpos_zero[0]) / MAX_VAL_CAR_0);
    scaling_factor[1] = abs((act_mot_pos_1 - motpos_zero[1]) / MAX_VAL_CAR_1);
    set_motor_positions(motpos_zero[0], motpos_zero[1], false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    motpos_zero[0] = motpos_zero[0] - (0.1138*pow(force_newtons, 3) - 5.204*pow(force_newtons, 2) + 89.22*force_newtons + 0) * scaling_factor[0];
    motpos_zero[1] = motpos_zero[1] + (0.1138*pow(force_newtons, 3) - 5.204*pow(force_newtons, 2) + 89.22*force_newtons + 0) * scaling_factor[1];
    set_motor_positions(motpos_zero[0], motpos_zero[1], false);
    std::cout << "Done" << std::endl;
}

int Cuff::io_thread_func() {
	while (io_loop_) {
        spinlock.lock();
		commSetInputs(&comm_settings_t_, CUFF_ID, reference_motor_positions_);
		commGetMeasurements(&comm_settings_t_, CUFF_ID, actual_motor_positions_);
		commGetCurrents(&comm_settings_t_, CUFF_ID, actual_motor_currents_);
        spinlock.unlock();
	}
    return 1;
}

void Cuff::set_motor_positions(short int motor_position_0, short int motor_position_1, bool immediate) {
    if (!immediate)
        spinlock.lock();
    reference_motor_positions_[0] = motor_position_0;
    reference_motor_positions_[1] = motor_position_1;
    if (!immediate)
        spinlock.unlock();
}

void Cuff::get_motor_positions(short int& motor_position_0, short int& motor_position_1) {
    spinlock.lock();
    motor_position_0 = actual_motor_currents_[0];
    motor_position_1 = actual_motor_currents_[1];
    spinlock.unlock();
}

void Cuff::get_motor_currents(short int& motor_current_0, short int& motor_current_1) {
    spinlock.lock();
    motor_current_0 = actual_motor_currents_[0];
    motor_current_1 = actual_motor_currents_[1];
    spinlock.unlock();
}
