#include "Motor.h"

namespace mel {

    Motor::Motor(double kt, double current_limit, double amp_gain, Daq::AoChannel ao_channel, Daq::DoChannel do_channel) :
        kt_(kt),
        current_limit_(current_limit),
        amp_gain_(amp_gain),
        ao_channel_(ao_channel),
        do_channel_(do_channel)
    {

    }

    void Motor::set_torque(double new_torque) {
        torque_ = new_torque;
        set_current(kt_*torque_);
    }

    void Motor::set_current(double new_current) {
        current_ = new_current;
        limited_current_ = limit_current(current_);
        ao_channel_.set_voltage(amp_gain_*limited_current_);
    }

    double Motor::limit_current(double new_current) {
        if (new_current > current_limit_) {
            return current_limit_;
        }
        else if (new_current < -current_limit_) {
            return -current_limit_;
        }
        else {
            return new_current;
        }

    }

    void Motor::enable() {
        is_enable_ = true;
        do_channel_.set_signal(0);
    }

    void Motor::disable() {
        is_enable_ = false;
        do_channel_.set_signal(1);
    }

}