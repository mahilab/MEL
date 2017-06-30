#pragma once
#include "Daq.h"

namespace mel {

    class Actuator {

    public:

        // parameters
        const double radius_{};
        const double kt_{};
        const double amp_gain_{};
        const double current_limit_{};
        const uint ao_channel_{};
        const uint do_channel_{};

        // relatives
        Daq* daq_;

        // constructors
        Actuator() {}
        Actuator(double radius, double kt, double amp_gain, double current_limit, Daq* daq, uint ao_channel, uint do_channel);
        //Actuator(double radius, double kt, double amp_gain, double current_limit, Daq::AoDoChannelSet ao_do_channel_set);

        // state variables
        double torque_;
        double current_;
        bool is_enable_;

        // setters of state variables
        void set_torque(double actuator_torque);
        void set_current(double current);

        void enable();
        void disable();
    };
}