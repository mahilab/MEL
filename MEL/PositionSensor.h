#pragma once
#include "Device.h"

namespace mel {

    class PositionSensor : public Device {

    public:

        //---------------------------------------------------------------------
        // CONSTRUCTOR(S) / DESTRUCTOR(S)
        //---------------------------------------------------------------------

        PositionSensor();
        PositionSensor(std::string name, bool velocity_enabled);

        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        /// This function should return the position of the PositionSensor
        virtual double get_position() = 0;
        /// This function should return the velocity of the PositionSensor
        virtual double get_velocity();

    protected:

        //---------------------------------------------------------------------
        // PROTECTED VARIABLES
        //---------------------------------------------------------------------

        bool velocity_enabled_; ///< whether or not this PositionSensor enables velocity measurement

        //---------------------------------------------------------------------
        // STATE VARIABLES
        //---------------------------------------------------------------------

        double position_; ///< stores the PositionSensor position since the last call to get_position()
        double velocity_; ///< stores the PositionSensor velocity since the last call to get_velocity()

    };
}