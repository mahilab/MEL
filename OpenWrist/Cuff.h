#pragma once
#include <thread>
#include "qbmove_communications.h"
#include "Spinlock.h"
#include "Device.h"

class Cuff : public mel::Device {

public:

    Cuff();
    Cuff(std::string name);
    ~Cuff() override;

	void enable() override;
    void disable() override;
	void set_motor_positions(short int motor_position_0, short int motor_position_1, bool immediate);
    void get_motor_positions(short int& motor_position_0, short int& motor_position_1);
    void get_motor_currents(short int& motor_current_0, short int& motor_current_1);

	void pretensioning(int force_newtons, short int* motpos_zero, short int* scaling_factor);

private:

	short int reference_motor_positions_[2];
	short int actual_motor_positions_[2];
	short int actual_motor_currents_[2];

	comm_settings comm_settings_t_;
    Spinlock spinlock;
    std::thread io_thread_;
    int io_thread_func();

	int open_port();
    int port_selection();

	volatile std::atomic_bool io_stop_flag_;

};
