#pragma once
#include "Actuator.h"

namespace mel {

    class Motor : public Actuator {

    public:

        // constructors
        Motor() {}
        Motor(double transmission, double kt, double current_limit, double amp_gain)
            : transmission_(transmission),
            kt_(kt),
            current_limit_(current_limit),
            amp_gain_(amp_gain)
        {

        }

        // setters of state variables
        void set_torque(double actuator_torque);
        void set_current(double current);
        double limit_current(double current_new);

        void enable();
        void disable();

    private:

        // parameters
        const double transmission_{};
        const double kt_{};
        const double amp_gain_{};
        const double cont_current_limit_{};
        const double peak_current_limit_{};
        const double i2t_current_limit_{};
        const uint32 ao_channel_{};
        const uint32 do_channel_{};

        

        // state variables
        double torque_;
        double current_;
        double limited_current_;
        double current_limit_;
        bool is_enable_;

        

    };
}