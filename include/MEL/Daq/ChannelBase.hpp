// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
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

#ifndef MEL_CHANNEL_HPP
#define MEL_CHANNEL_HPP

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
    ChannelBase() : module_(nullptr), channel_number_(0) {}

    /// Creates a valid channel.
    ChannelBase(Module<T>* module, uint32 channel_number)
        : module_(module), channel_number_(channel_number) {}

    /// Destructor
    virtual ~ChannelBase() {}

    /// Synchronizes the channel with the real-world
    bool update() { return module_->update_channel(channel_number_); }

    /// Returns the current value of the channel
    T get_value() const { return module_->get_value(channel_number_); }

    /// Sets the current value of the channel
    void set_value(T value) { module_->set_value(channel_number_, value); }

    /// Overload assignment operator for setting
    void operator= (const T& value) {
        set_value(value);
    }

    /// Overload cast opoerator for getting
    operator T() const {
        return get_value();
    }

    /// Gets the channel number
    uint32 get_channel_number() const { return channel_number_; }

    /// Returns true if the Channel is valid
    bool is_valid() const {
        if (module_ == nullptr)
            return false;
        if (!module_->validate_channel_number(channel_number_))
            return false;
        return true;
    }

protected:
    Module<T>* module_;      ///< Pointer to the module this channel is on
    uint32 channel_number_;  ///< The channel number of this channel
};


}  // namespace mel

#endif  // MEL_CHANNEL_HPP
