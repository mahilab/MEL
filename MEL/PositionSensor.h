#pragma once

namespace mel {

    class PositionSensor {

    public:

        // constructor
        PositionSensor() {}

        virtual double get_position() = 0;
        virtual double get_velocity() {}

    protected:

        double position_;
        double velocity_;

    };
}