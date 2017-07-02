#pragma once

namespace mel {

    class PositionSensor {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        PositionSensor();
        PositionSensor(bool velocity_enabled) : velocity_enabled_(velocity_enabled) {}

        // PUBLIC FUNCTIONS

        virtual double get_position() = 0;
        virtual double get_velocity();

    protected:

        // PROTECTED VARIABLES

        bool velocity_enabled_; // whether or not this PositionSensor enables velocity measurement

        // STATE VARIABLES

        double position_;
        double velocity_;

    };
}