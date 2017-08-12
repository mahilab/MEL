#pragma once
#include "Daq.h"

namespace mel {

    class Actuator {

        

    public:

        enum class EnableMode { None, Low, High };

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        Actuator();
        Actuator(std::string name, EnableMode enable_mode);

        // PUBLIC FUNCTIONS

        virtual void set_torque(double actuator_torque) = 0;

        virtual void enable();
        virtual void disable();

        // PUBLIC VARIABLES

        const std::string name_;
        bool enabled_; // whether or not the Actuator has been enabled


    protected:

        // PROTECTED VARIABLES

        EnableMode enable_mode_;

        // STATE VARIABLES

        double torque_; // stores the Actuator torque since the last call to set_torque()

    };
}