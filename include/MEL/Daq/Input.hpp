// MIT License
//
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

#ifndef MEL_INPUT_HPP
#define MEL_INPUT_HPP

#include <MEL/Daq/Module.hpp>
#include <MEL/Daq/ChannelBase.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Input only Module
template <typename T>
class Input : public Module<T> {

public:

    class Channel;

    /// Default constructor
    Input(const std::string& name, const std::vector<uint32>& channel_numbers) :
        Module<T>(name, IoType::InputOnly, channel_numbers)
    {
    }

    /// Default destructor
    virtual ~Input() { }

    Channel get_channel(uint32 channel_number) {
        if (Module<T>::validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    std::vector<Channel> get_channels(const std::vector<uint32>& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(get_channel(channel_numbers[i]));
        return channels;
    }

    Channel operator[](uint32 channel_number) {
        return get_channel(channel_number);
    }

    std::vector<Channel> operator[](const std::vector<uint32>& channel_numbers) {
        return get_channels(channel_numbers);
    }

    /// Encapsulates a Module channel
    class Channel : public ChannelBase<T, Input<T>> {

    public:

        /// Default constructor. Creates invalid channel
        Channel() : ChannelBase<T, Input<T>>() {}

        /// Creates a valid channel.
        Channel(Input* module, uint32 channel_number) :
            ChannelBase<T, Input<T>>(module, channel_number) { }

    };

};

//==============================================================================
// TYPEDEFS
//==============================================================================

typedef Input<voltage> AnalogInput;
typedef Input<logic>   DigitalInput;

} // namespace mel

#endif // MEL_INPUT_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
