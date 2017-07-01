#pragma once
#include "Daq.h"

namespace mel {

    class Actuator {

    public:


        // constructors
        Actuator();

        // setters of state variables
        void set_torque(double actuator_torque);


        void enable();
        void disable();
    };
}