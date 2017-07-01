#pragma once
#include "Daq.h"

namespace mel {

    class Actuator {

    public:

        // constructors
        Actuator() {}

        virtual void set_torque(double actuator_torque) = 0;

        virtual void enable();
        virtual void disable();

    protected:

        double torque_;
    };
}