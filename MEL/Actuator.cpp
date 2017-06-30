#include "Actuator.h"

namespace mel {

   
    //Actuator::Actuator(double radius, double kt, double amp_gain, double current_limit, Daq::AoDoChannelSet ao_do_channel_set) :
    //    Actuator(radius, kt, amp_gain, current_limit, ao_do_channel_set.da)
    

    void Actuator::set_torque(double actuator_torque) {
        torque_ = actuator_torque;
        set_current(kt_*actuator_torque);
    }

    void Actuator::set_current(double current) {
        current_ = current;
        limited_current_ = limit_current(current_);
        daq_->set_analog_voltage(ao_channel_, amp_gain_*limited_current_);
    }

    double Actuator::limit_current(double current_new) {
        if (current_new > current_limit_) {
            return current_limit_;
        }
        else if ( current_new < -current_limit_) {
            return -current_limit_;
        }
        else {
            return current_new;
        }

    }

    void Actuator::enable() {
        daq_->set_digital_state(do_channel_, '0');
    }

    void Actuator::disable() {
        daq_->set_digital_state(do_channel_, '1');
    }
}