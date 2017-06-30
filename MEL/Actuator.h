#pragma once
#include "Daq.h"

namespace mel {

    class Actuator {

    public:

        // parameters
        const double radius_{};
        const double kt_{};
        const double amp_gain_{};
        const double cont_current_limit_{};
        const double peak_current_limit_{};
        const double i2t_current_limit_{};
        const uint ao_channel_{};
        const uint do_channel_{};

        // relatives
        Daq* daq_;

        // constructors
        Actuator() {}
        //Actuator(double radius, double kt, double amp_gain, double current_limit, Daq::AoDoChannelSet ao_do_channel_set);
        Actuator(double radius, double kt, double current_limit, double amp_gain, Daq* daq, uint ao_channel, uint do_channel)
            : radius_(radius),
            kt_(kt),
            current_limit_(current_limit),
            amp_gain_(amp_gain),
            ao_channel_(ao_channel),
            do_channel_(do_channel),
            daq_(daq) {}

        // state variables
        double torque_;
        double current_;
        double limited_current_;
        double current_limit_;
        bool is_enable_;

        // setters of state variables
        void set_torque(double actuator_torque);
        void set_current(double current);
        double limit_current(double current_new);

        void enable();
        void disable();
    };
}