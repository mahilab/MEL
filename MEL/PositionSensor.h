#pragma once

namespace mel {

    class PositionSensor {

    public:

        // CONSTRUCTOR / DESTRUCTOR

        PositionSensor();

        // PUBLIC FUNCTIONS

        virtual double get_position() = 0;
        virtual double get_velocity() { return velocity_; }

    protected:

        // STATE VARIABLES

        double position_;
        double velocity_;

    };
}