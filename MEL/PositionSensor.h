#pragma once
#include <string>
#include <iostream>

namespace mel {

    class PositionSensor {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        PositionSensor();
        PositionSensor(std::string name, bool velocity_enabled);

        // PUBLIC FUNCTIONS

        virtual void enable() {}
        virtual void disable() {}

        virtual double get_position() = 0;
        virtual double get_velocity();

        // PUBLIC VARIABLES

        std::string name_;

    protected:

        // PROTECTED VARIABLES

        bool velocity_enabled_; // whether or not this PositionSensor enables velocity measurement

        // STATE VARIABLES

        double position_; // stores the PositionSensor position since the last call to get_position()
        double velocity_; // stores the PositionSensor velocity since the last call to get_velocity()

    };
}