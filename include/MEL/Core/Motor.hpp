#pragma once

#include <MEL/Core/Actuator.hpp>
#include <MEL/Core/Limiter.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a (typically DC) Motor actuator
class Motor : public Actuator {

public:

    /// Default constructor
    Motor();

    /// Constructor for digitally enabled, analog voltage controlled DC motor
    Motor(std::string name,
        double kt,
        double amp_gain,
        EnableMode enable_mode,
        Output<logic>::Channel do_channel,
        Output<voltage>::Channel ao_channel,
        Limiter current_limiter = Limiter());

    /// Constructor for digitally enabled, analog voltage controlled DC motor
    /// with analog voltage current sensing
    Motor(std::string name,
        double kt,
        double amp_gain,
        EnableMode enable_mode,
        Output<logic>::Channel do_channel,
        Output<voltage>::Channel ao_channel,
        Input<voltage>::Channel ai_channel,
        Limiter current_limiter = Limiter());

    /// Enables the Motor using the associated digital output channel
    bool enable() override;

    /// Disables the Motor using using the associated digital output channel and writes zero to the associated analog output channel
    bool disable() override;

    /// Sets the desired torque to be generated at the Motor, converts from torque to current, and calls set_current()
    void set_torque(double torque) override;

    /// Returns the attempted command current since the last call to set_current()
    double get_torque_command() override;

    /// Returns the limited command current since the last call to set_current()
    double get_torque_limited() override;

    ///Returns the torque sense of the motor if it is available
    double get_torque_sense() override;

    /// Set the desired current to be generated at the Motor
    void set_current(double current);

    /// Returns the commanded current resulting from the commanded current
    double get_current_command();

    /// Returns the limited current resulting from the current limit mode operation
    double get_current_limited();

    /// Gets the current measured by the current sensor of the current amplifier
    double get_current_sense();

private:

    double kt_;                            ///< torque constant of the Motor
    double kt_inv_;                        ///< inverse torque constant of the Motor
    double amp_gain_;                      ///< amplifier gain [A/V]

    Output<logic>::Channel   do_channel_;  ///< the DAQ digital output channel bound to this Motor (for enable/disable)
    Output<voltage>::Channel ao_channel_;  ///< the DAQ analog output channel bound to this Motor (for commanding torque/current)
    Input<voltage>::Channel  ai_channel_;  ///< the DAQ analog input channel bound to this Motor (for torque/current sensing)

    Limiter current_limiter_;              ///< The Motor current Limiter
    double current_command_;               ///< stores the desired Motor current since the last call to set_current()
    double current_sense_;                 ///< store the measured current since the last call to get_current_sense()

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
