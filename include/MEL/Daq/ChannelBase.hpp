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
#include <MEL/Core/Types.hpp>
#include <MEL/Daq/Module.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

template <typename T>
class ChannelBase {
public:
    /// Default constructor. Creates invalid channel
    ChannelBase();

    /// Creates a valid channel.
    ChannelBase(Module<T>* module, ChanNum channel_number);

    /// Destructor
    virtual ~ChannelBase();

    /// Synchronizes the channel with the real-world
    bool update();

    /// Returns the current value of the channel
    T get_value() const;

    /// Sets the current value of the channel
    void set_value(T value);

    /// Overload assignment operator for setting
    void operator= (const T& value);

    /// Overload cast opoerator for getting
    operator T() const;

    /// Gets the channel number
    ChanNum get_channel_number() const;

    /// Returns true if the Channel is valid
    bool is_valid() const;

protected:
    Module<T>* module_;       ///< Pointer to the module this channel is on
    ChanNum channel_number_;  ///< The channel number of this channel
};

}  // namespace mel

#include <MEL/Daq/Detail/ChannelBase.inl>
