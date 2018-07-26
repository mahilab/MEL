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

#ifndef MEL_INPUTOUTPUT_HPP
#define MEL_INPUTOUTPUT_HPP

#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an InputOutput only Module, with added functionality
template <typename T>
class InputOutput : public Input<T>, public Output<T> {
public:


    class Channel;

    /// Default constructor
    InputOutput(const std::string& name, const std::vector<uint32>& channel_numbers, const std::vector<Direction>& directions) :
        Module<T>(name, IoType::InputOutput, channel_numbers),
        Input<T>(name, channel_numbers),
        Output<T>(name, channel_numbers),
        directions_(directions),
        enable_values_(Module<T>::channel_count_),
        disable_values_(Module<T>::channel_count_),
        expire_values_(Module<T>::channel_count_)
    {
        sort_input_output_channel_numbers();
    }

    /// Default destructor
    virtual ~InputOutput() {}

    /// Sets the directions of all channels
    virtual bool set_directions(const std::vector<Direction>& directions) {
        if (this->validate_channel_count(directions.size())) {
            directions_ = directions;
            sort_input_output_channel_numbers();
            return true;
        }
        return false;
    }

    /// Sets the direction of a single channel
    virtual bool set_direction(uint32 channel_number, Direction direction) {
        if (this->validate_channel_number(channel_number)) {
            directions_[Module<T>::channel_map_.at(channel_number)] = direction;
            sort_input_output_channel_numbers();
            return true;
        }
        return false;
    }

public:

    /// Gets a handle to a channel on this module
    Channel get_channel(uint32 channel_number) {
        if (Module<T>::validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    /// Gets a vector of handles to channels on this module
    std::vector<Channel> get_channels(
        const std::vector<uint32>& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(get_channel(channel_numbers[i]));
        return channels;
    }

    /// Gets a handle to a channel on this module
    Channel operator[](uint32 channel_number) {
        return get_channel(channel_number);
    }

    /// Gets a vector of handles to channels on this module
    std::vector<Channel> operator[](
        const std::vector<uint32>& channel_numbers) {
        return get_channels(channel_numbers);
    }

protected:
    /// Sorts channel numbers associated with inputs and outputs
    void sort_input_output_channel_numbers() {
        input_channel_numbers_.clear();
        output_channel_numbers_.clear();
        for (std::size_t i = 0; i < this->channel_numbers_.size(); ++i) {
            if (directions_[i] == In)
                input_channel_numbers_.push_back(this->channel_numbers_[i]);
            else if (directions_[i] == Out)
                output_channel_numbers_.push_back(this->channel_numbers_[i]);
        }
    }

protected:
    std::vector<Direction> directions_;           ///< The I/O directions of each channel
    std::vector<uint32> input_channel_numbers_;   ///< the channel numbers that are inputs
    std::vector<uint32> output_channel_numbers_;  ///< the channel numbers that are outputs
    std::vector<T>  enable_values_;               ///< The initial values set when the Module is enabled
    std::vector<T> disable_values_;               ///< The final values set when the Module is disabled
    std::vector<T> expire_values_;                ///< The expire values when the Module expires

public:

    /// Encapsulates a Module channel
    class Channel : public Input<T>::Channel, public Output<T>::Channel {
    public:
        /// Default constructor. Creates invalid channel
        Channel() : ChannelBase<T>() {}

        /// Creates a valid channel.
        Channel(InputOutput* module, uint32 channel_number)
            : ChannelBase<T>(module, channel_number) {}

        /// Sets the direction of the channel
        void set_direction(Direction direction) {
           dynamic_cast<InputOutput<T>*>(this->module_)->set_direction(this->channel_number_, direction);
        }

    };
};

//==============================================================================
// TYPEDEFS
//==============================================================================

typedef InputOutput<Logic> DigitalInputOutput;

}  // namespace mel

#endif  // MEL_INPUTOUTPUT_HPP
