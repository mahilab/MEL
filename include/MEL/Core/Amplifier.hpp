#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Core/Limiter.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a current controlled servo amplifier
class Amplifier : public Device {

public:

    /// Represents a digital TTL logic level
    enum TtlLevel {
        High,  ///< High TTL level (typically 5V)
        Low,   ///< Low TTL level  (typically 0V)
    };

public:

    /// Default constructor
    Amplifier(const std::string& name,
        TtlLevel enable_level,
        DigitalOutput::Channel enable_channel,
        double command_gain,
        AnalogOutput::Channel command_channel,
        Limiter current_limiter = Limiter(),
        TtlLevel fault_level = High,
        DigitalInput::Channel fault_channel = DigitalInput::Channel(),
        double sense_gain = 1.0,
        AnalogInput::Channel sense_channel = AnalogInput::Channel());

    /// Enables the Amplifier using the digital output channel
    bool enable() override;

    /// Zeros command current and disables the Amplifier using the digital output channel
    bool disable() override;

    /// Set the desired current to be produced by the Amplifier
    void set_current(double current);

    /// Returns the commanded current since the last call to set_current()
    double get_current_command() const;

    /// Returns the limited current resulting from the current limit mode operation
    double get_current_limited() const;

    /// Gets the actual current measured by the amplifier
    double get_current_sense() const;

    /// Returns true if the Amplifier is in a faulted state
    bool is_faulted(bool force_update = false);

private:

    TtlLevel enable_level_; ///< The TTL level required to enable to Amplifier
    TtlLevel fault_level_;  ///< The TTL level expected when the Amplifier faults

    double command_gain_;  ///< the input voltage to output current gain [A/V]
    double sense_gain_;    ///< the gain on the output sense voltage [V/V]

    DigitalOutput::Channel enable_channel_;   ///< digital output channel which enables
    DigitalInput::Channel  fault_channel_;    ///< digital input channel which faults
    AnalogOutput::Channel  command_channel_;  ///< analog output channel which commands
    AnalogInput::Channel   sense_channel_;    ///< analog input channel which senses

    double current_command_;  ///< stores the desired Amplifier current since the last call to set_current()

    Limiter current_limiter_;  ///< The Motor current Limiter

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
