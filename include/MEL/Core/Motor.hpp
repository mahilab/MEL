#pragma once

#include <MEL/Core/Actuator.hpp>
#include <MEL/Core/Amplifier.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a DC Motor
class Motor : public Actuator {

public:


    /// constructor
    Motor(const std::string& name, double kt, Amplifier amplifier, Limiter current_limiter = Limiter());

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

    /// Returns the torque sense of the motor if it is available
    double get_torque_sense() override;

private:

    double kt_;                ///< torque constant of the Motor
    Amplifier amplifier_;      ///< the current amplifier controlling the motor
    Limiter current_limiter_;  ///< the Motor current limiter

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
