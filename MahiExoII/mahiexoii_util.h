#pragma once
#include "util.h"
#include "Daq.h"
#include "Q8Usb.h"
#include "Clock.h"


bool check_digital_loopback(mel::uint32 daq_id, mel::channel digital_channel);

double moving_set_point(double init_pos, double goal_pos, double init_time, double time, double speed);