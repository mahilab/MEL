#pragma once
#include "Device.h"
#include "Daq.h"

namespace mel {

    class Actuator : public Device {        

    public:

        enum class EnableMode { None, Low, High };

        //---------------------------------------------------------------------
        // CONSTRUCTOR(S) / DESTRUCTOR(S)
        //---------------------------------------------------------------------

        /// Default constructor
        Actuator();
        /// Constructor for actuator without torque limits
        Actuator(std::string name, EnableMode enable_mode);


        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        /// This function should set the desired torque to be generated at the Actuator
        virtual void set_torque(double actuator_torque) = 0;
        /// This function should return the attempted command current
        virtual double get_command_torque() { return torque_; }
        /// This function should return the limited command current if limitations are imposed
        virtual double get_limited_torque() { return torque_; }

        //---------------------------------------------------------------------
        // PUBLIC VARIABLES
        //---------------------------------------------------------------------


    protected:

        //---------------------------------------------------------------------
        // PROTECTED VARIABLES
        //---------------------------------------------------------------------

        EnableMode enable_mode_; ///< the enable mode of the actuator (None, Low, or High)

        double torque_; ///< stores the Actuator torque since the last call to set_torque()


    };
}