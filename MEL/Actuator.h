#pragma once
#include "Device.h"

namespace mel {

    namespace core {

        class Actuator : public Device {

        public:

            /// The digital logic level required to enable the Actuator (i.e. High
            /// is typically 5V or 10V, and Low is usually 0V). 
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
            virtual double get_torque_command() { return torque_; }
            /// This function should return the limited command current if limitations are imposed
            virtual double get_torque_limited() { return torque_; }
            /// This function should return the torque sense of the actuator if it is available
            virtual double get_torque_sense() { return 0.0; }

        protected:

            //---------------------------------------------------------------------
            // PROTECTED VARIABLES
            //---------------------------------------------------------------------

            EnableMode enable_mode_; ///< The enable mode of the actuator (None, Low, or High)
            double torque_; ///< Stores the Actuator torque since the last call to set_torque()

            double torque_sense_; ///< Stores the Actuator torque sense since the last call to get_torque_sense();
            bool has_torque_sense_; ///< True if the Actuator has torque sensing capabilities

        };

    }
}