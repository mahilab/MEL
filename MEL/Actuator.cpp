#include "Actuator.h"

namespace mel {

    Actuator::Actuator(double radius, double kt, double amp_gain, double current_limit, Daq* daq, uint ao_channel, uint do_channel)
        : radius_(radius),
        kt_(kt),
        amp_gain_(amp_gain),
        current_limit_(current_limit),
        ao_channel_(ao_channel),
        do_channel_(do_channel),
        daq_(daq) {}

    
    //Actuator::Actuator(double radius, double kt, double amp_gain, double current_limit, Daq::AoDoChannelSet ao_do_channel_set) :
    //    Actuator(radius, kt, amp_gain, current_limit, ao_do_channel_set.da)
    

    void Actuator::set_torque(double actuator_torque) {
        torque_ = actuator_torque;
        set_current(kt_*actuator_torque);
    }

    void Actuator::set_current(double current) {
        current_ = current;
        daq_->set_analog_voltage(ao_channel_, amp_gain_*current);
    }

    void Actuator::enable() {
        daq_->set_digital_state(do_channel_, '0');
    }

    void Actuator::disable() {
        daq_->set_digital_state(do_channel_, '1');
    }
}