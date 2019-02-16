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

#pragma once

#include <MEL/Daq/ChannelBase.hpp>
#include <MEL/Daq/Module.hpp>
namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Output only Module, with added functionality
template <typename T>
class Output : virtual public Module<T> {
public:

    class Channel;

    /// Default Constructor (creates an invlaid empty Output Module)
    Output();

    /// Constructor with specified channel numbers
    Output(const std::vector<uint32>& channel_numbers);

    /// Destructor
    virtual ~Output();

    /// Override to set watchdog expiration state
    virtual bool set_expire_values(const std::vector<T>& expire_values);

    /// Override to set watchdog expiration state of a single channel
    virtual bool set_expire_value(uint32 channel_number, T expire_value);

    /// Sets the initial values to be written on enable
    void set_enable_values(const std::vector<T>& enable_values);

    /// Sets the intial value of a single channel to be written on enable
    void set_enable_value(uint32 channel_number, T enable_value);

    /// Sets the final values to be written on disable
    void set_disable_values(const std::vector<T>& disable_values);

    /// Sets the final value of a single channel to be written on disable
    void set_disable_value(uint32 channel_number, T disable_value);

    /// Gets a handle to a channel on this module
    Channel get_channel(uint32 channel_number);

    /// Gets a vector of handles to channels on this module
    std::vector<Channel> get_channels(const std::vector<uint32>& channel_numbers);

    /// Gets a handle to a channel on this module
    Channel operator[](uint32 channel_number);

    /// Gets a vector of handles to channels on this module
    std::vector<Channel> operator[](const std::vector<uint32>& channel_numbers);

protected:

    /// Sets values to enable values and updates
    virtual bool on_enable();

    /// Sets values to disable values and updates
    virtual bool on_disable();

protected:
    Registry<T> enable_values_;   ///< The initial values set when the Module is enabled
    Registry<T> disable_values_;  ///< The final values set when the Module is disabled
    Registry<T> expire_values_;   ///< The expire values when the Module expires

public:
    /// Encapsulates a Module channel
    class Channel : virtual public ChannelBase<T> {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(Output* module, uint32 channel_number);

        /// Inherit assignment operator for setting
        using ChannelBase<T>::operator=;

        /// Sets the enable value of the channel
        void set_enable_value(T enable_value);

        /// Sets the disable value of the channel
        void set_disable_value(T disable_value);

        /// Sets the expiration value of the channel
        bool set_expire_value(T expire_value);
    };
};

//==============================================================================
// TYPEDEFS
//==============================================================================

typedef Output<Voltage> AnalogOutput;
typedef Output<Logic> DigitalOutput;

}  // namespace mel

#include <MEL/Daq/Detail/Output.inl>
