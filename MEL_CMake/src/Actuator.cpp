#include "../lib/Actuator.h"

namespace mel {

    void Actuator::set_torque(double torque) {
        torque_ = torque;
        set_current(kt_*torque);
    }

    void Actuator::set_current(double current) {
        current_ = current;
        set_command(amp_gain_*current);
    }

    void Actuator::set_command(double voltage) {
        daq_->set_analog_voltage(ao_channel_, voltage);
    }
}