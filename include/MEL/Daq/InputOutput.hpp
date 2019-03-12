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
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an InputOutput only Module
template <typename T>
class InputOutput : public Output<T> {
public:

    class Channel;

    /// Default Constructor (creates an invlaid empty InputOutput Module)
    InputOutput();

    /// Constructor with specified channel numbers
    InputOutput(const ChanNums& channel_numbers);

    /// Default destructor
    virtual ~InputOutput();

    /// Updates only channels configured as inputs
    virtual bool update_input();

    /// Updates only channels configured as outputs
    virtual bool update_output();

    /// Sets the direction of a single channel
    virtual bool set_direction(ChanNum channel_number, Direction direction) = 0;

    /// Sets the directions of all channels
    virtual bool set_directions(const std::vector<Direction>& directions);

public:

    /// Returns the number of input channels on this Module
    std::size_t get_input_channel_count() const;

    /// Returns the number of output channels on this Module
    std::size_t get_output_channel_count() const;
    
    /// Gets the vector of input channel numbers this Module maintains
    const ChanNums& get_input_channel_numbers() const;

    /// Gets the vector of output channel numbers this Module maintains
    const ChanNums& get_output_channel_numbers() const;

    /// Gets a handle to a channel on this module
    Channel get_channel(ChanNum channel_number);

    /// Gets a vector of handles to channels on this module
    std::vector<Channel> get_channels(const ChanNums& channel_numbers);

    /// Gets a handle to a channel on this module
    Channel operator[](ChanNum channel_number);

    /// Gets a vector of handles to channels on this module
    std::vector<Channel> operator[](const ChanNums& channel_numbers);

protected:

    /// Sets the channel numbers this Module maintains
    virtual void set_channel_numbers(const ChanNums& channel_numbers) override;

    /// Adds a channel number to current channel numbers
    virtual void add_channel_number(ChanNum channel_number) override;

    /// Removes a channel number from current channel numbers
    virtual void remove_channel_number(ChanNum channel_number) override;

    /// Sorts channel numbers associated with inputs and outputs
    void sort_input_output_channel_numbers() const;

protected:
    Registry<Direction> directions_;           ///< The I/O directions of each channel
    mutable ChanNums input_channel_numbers_;   ///< the channel numbers that are inputs
    mutable ChanNums output_channel_numbers_;  ///< the channel numbers that are outputs

public: 

    /// Encapsulates a Module channel
    class Channel : public Input<T>::Channel, public Output<T>::Channel {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(InputOutput* module, ChanNum channel_number);

        /// Inherit assignment operator for setting
        using ChannelBase<T>::operator=;

        /// Sets the direction of the channel
        void set_direction(Direction direction);
    };
};

//==============================================================================
// TYPEDEFS
//==============================================================================

typedef InputOutput<Logic> DigitalInputOutput;

}  // namespace mel

#include <MEL/Daq/Detail/InputOutput.inl>
