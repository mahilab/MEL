// MIT License
//
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab, Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_AMPLIFIER_HPP
#define MEL_AMPLIFIER_HPP

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
    /// Constructor
    ///
    /// @param name the name of the amplifier
    /// @param enable_level TTL level required to enable device
    /// @param enable_channel the DO channel which enables the Amplifier
    /// @param command_gain the command gain in [A/V]
    /// @param command_channel the AO channel which commands current
    /// @param current_limiter Limiter protected Amplifier
    /// @param fault_level TTL level expected when the Amplifier faults
    /// @param fault_channel the DI channel which reads in a fault
    /// @param sense_gain the sense gain in [V/V]
    /// @param sense_channel the AI channel which reads in sensed current
    Amplifier(const std::string& name,
              TtlLevel enable_level,
              DigitalOutput::Channel enable_channel,
              double command_gain,
              AnalogOutput::Channel command_channel,
              Limiter current_limiter             = Limiter(),
              TtlLevel fault_level                = High,
              DigitalInput::Channel fault_channel = DigitalInput::Channel(),
              double sense_gain                   = 1.0,
              AnalogInput::Channel sense_channel  = AnalogInput::Channel());

    /// Enables the Amplifier using the DO enable channel
    ///
    /// @return true if successful, false otherwise
    bool enable() override;

    /// Disables the Amplifier using the DO enable channel
    ///
    /// @return true if successful, false otherwise
    bool disable() override;

    /// Sets the desired current to be produced by the Amplifier
    ///
    /// @param current the current to be set [A]
    void set_current(double current);

    /// Returns the last current value commanded
    ///
    /// @return the last current value commanded [A]
    double get_current_command() const;

    /// Returns the limited version of the last current value commanded
    ///
    /// @return the last limited version of the last current value commanded [A]
    double get_current_limited() const;

    /// Gets the actual current measured by the amplifier
    ///
    /// @return the actual current measured by the amplifier [A]
    double get_current_sense() const;

    /// Returns true if the Amplifier is in a faulted state
    ///
    /// @param force_update if true, fault_channel will be explicitly updated
    /// @return true if the Amplifier is faulted, false otherwise
    bool is_faulted(bool force_update = false);

private:
    TtlLevel enable_level_;  ///< TTL enable level
    TtlLevel fault_level_;   ///< TTL fault level

    double command_gain_;  ///< command gain [A/V]
    double sense_gain_;    ///< sense gain [V/V]

    DigitalOutput::Channel enable_channel_;  ///< DO enable channel
    DigitalInput::Channel fault_channel_;    ///< DI fault channel
    AnalogOutput::Channel command_channel_;  ///< AO current command channel
    AnalogInput::Channel sense_channel_;     ///< AI current sense channel

    double current_command_;   ///< current command value
    Limiter current_limiter_;  ///< current limiter protecting Amplifier
};

}  // namespace mel

#endif  // MEL_AMPLIFIER_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
