#include "Actuator.h"

namespace mel {

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