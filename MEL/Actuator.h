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
        /// Constructor for actuator with torque limits
        Actuator(std::string name, EnableMode enable_mode, double torque_limit);

        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        /// This function should set the desired torque to be generated at the Actuator
        virtual void set_torque(double actuator_torque) = 0;

    protected:

        //---------------------------------------------------------------------
        // PROTECTED VARIABLES
        //---------------------------------------------------------------------

        EnableMode enable_mode_; ///< the enable mode of the actuator (None, Low, or High)

        double torque_; ///< stores the Actuator torque since the last call to set_torque()
        double torque_limit_; ///< the absolute limit of torque that should be allowed from the Actuator
        bool has_torque_limit_; /// whether or not the Actuator enforces a torque limit


    };
}