//==============================================================================
// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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
//==============================================================================

#pragma once

#include <MEL/Core/Device.hpp>
#include <MEL/Mechatronics/Limiter.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>

namespace mel {

//==============================================================================
/// Encapsulates a DC current amplifier
//==============================================================================
class Amplifier : public Device {

public:

    //==========================================================================
    /// Defualt Constructor
    //==========================================================================
    Amplifier();

    //==========================================================================
    /// Full Constructor
    ///
    /// \param name the name of the amplifier
    /// \param enable_level TTL level required to enable device
    /// \param enable_channel the DO channel which enables the Amplifier
    /// \param command_gain the command gain in [A/V]
    /// \param command_channel the AO channel which commands current
    /// \param current_limiter current Limiter protecting Amplifier
    /// \param fault_level TTL level expected when the Amplifier faults
    /// \param fault_channel the DI channel which reads in a fault
    /// \param sense_gain the sense gain in [A/V]
    /// \param sense_channel the AI channel which reads in sensed current
    //==========================================================================
    Amplifier(const std::string& name,
              Logic enable_level,
              DigitalOutput::Channel enable_channel,
              double command_gain,
              AnalogOutput::Channel command_channel,
              Limiter current_limiter             = Limiter(),
              Logic fault_level                   = High,
              DigitalInput::Channel fault_channel = DigitalInput::Channel(),
              double sense_gain                   = 1.0,
              AnalogInput::Channel sense_channel  = AnalogInput::Channel());

    //==========================================================================
    /// Sets the digital output channel which enables the Amplifer.
    //==========================================================================
    void bind_enable_channel(DigitalOutput::Channel enable_channel);

    //==========================================================================
    /// Sets the analog output channel which commands current.
    //==========================================================================
    void bind_command_channel(AnalogOutput::Channel command_channel);

    //==========================================================================
    /// Sets the digital input channel which reads in a fault.
    //==========================================================================
    void bind_fault_channel(DigitalInput::Channel fault_channel);

    //==========================================================================
    /// Sets the analog input channel which reads in sensed current.
    //==========================================================================
    void bind_sense_channel(AnalogInput::Channel sense_channel);

    //==========================================================================
    /// Sets the TTL level required to enable device.
    //==========================================================================
    void set_enable_level(Logic enable_level);

    //==========================================================================
    /// Sets the TTL level expected when the Amplifier faults.
    //==========================================================================
    void set_fault_level(Logic fault_level);

    //==========================================================================
    /// Sets the analog command gain in [A/V]
    //==========================================================================
    void set_command_gain(double command_gain);

    //==========================================================================
    /// Sets the analog sense gain in [A/V]
    //==========================================================================
    void set_sense_gain(double sense_gain);

    //==========================================================================
    /// Sets the Limiter protecting over current
    //==========================================================================
    void set_limiter(Limiter current_limiter);

    //==========================================================================
    /// Sets the desired current [A] to be produced by the Amplifier
    //==========================================================================
    void set_current(double current);

    //==========================================================================
    /// Returns the last current value commanded
    //==========================================================================
    double get_current_command() const;

    //==========================================================================
    /// Returns the limited version of the last current value commanded
    //==========================================================================
    double get_current_limited() const;

    //==========================================================================
    /// Gets the actual current measured by the amplifier
    //==========================================================================
    double get_current_sense() const;

    //==========================================================================
    /// Returns true if the Amplifier is in a faulted state
    ///
    /// \param force_update if true, fault_channel will be explicitly updated
    /// \return true if the Amplifier is faulted, false otherwise
    //==========================================================================
    bool is_faulted(bool force_update = false);

protected:

    //==========================================================================
    /// Enables the Amplifier using the DO enable channel
    ///
    /// \return true if successful, false otherwise
    //==========================================================================
    bool on_enable() final;

    //==========================================================================
    /// Disables the Amplifier using the DO enable channel
    ///
    /// \return true if successful, false otherwise
    //==========================================================================
    bool on_disable() final;

protected:
    Logic enable_level_;  ///< TTL enable level
    Logic fault_level_;   ///< TTL fault level

    double command_gain_;  ///< command gain [A/V]
    double sense_gain_;    ///< sense gain [A/V]

    DigitalOutput::Channel enable_channel_;  ///< DO enable channel
    DigitalInput::Channel fault_channel_;    ///< DI fault channel
    AnalogOutput::Channel command_channel_;  ///< AO current command channel
    AnalogInput::Channel sense_channel_;     ///< AI current sense channel

    double current_command_;   ///< current command value
    Limiter current_limiter_;  ///< current limiter protecting Amplifier
};

}  // namespace mel
