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

#ifndef MEL_OUTPUT_HPP
#define MEL_OUTPUT_HPP

#include <MEL/Daq/ChannelBase.hpp>
#include <MEL/Daq/Module.hpp>

namespace mel {

//==============================================================================
// CLASS DEClARATION
//==============================================================================

/// Encapsulates an Output only Module, with added functionality
template <typename T>
class Output : public Module<T> {
public:
    class Channel;

    /// Constructor
    Output(const std::string& name, const std::vector<uint32>& channel_numbers)
        : Module<T>(name, IoType::OutputOnly, channel_numbers),
          enable_values_(Module<T>::channel_count_),
          disable_values_(Module<T>::channel_count_),
          expire_values_(Module<T>::channel_count_) {}

    /// Default destructor
    virtual ~Output() {}

    /// This function should call the DAQ's API to set watchdog expire values if
    /// the functionality exists. The vector of expire values must correspond to
    /// the enabled channel numbers in acending order. The base implementation
    /// below should be called in the derived implementation.
    virtual bool set_expire_values(const std::vector<T>& expire_values) {
        if (Module<T>::validate_channel_count(expire_values.size())) {
            expire_values_ = expire_values;
            return true;
        }
        return false;
    }

    /// Sets the expire value of a single channel
    virtual bool set_expire_value(uint32 channel_number, T expire_value) {
        if (Module<T>::validate_channel_number(channel_number)) {
            expire_values_[Module<T>::channel_map_.at(channel_number)] =
                expire_value;
            return true;
        }
        return false;
    }

public:
    /// Sets the initial values to be written on enable
    void set_enable_values(const std::vector<T>& enable_values) {
        if (validate_channel_count(enable_values.size()))
            enable_values_ = enable_values;
    }

    /// Sets the intial value of a single channel to be written on enable
    void set_enable_value(uint32 channel_number, T enable_value) {
        if (Module<T>::validate_channel_number(channel_number))
            enable_values_[Module<T>::channel_map_.at(channel_number)] =
                enable_value;
    }

    /// Sets the final values to be written on disable
    void set_disable_values(const std::vector<T>& disable_values) {
        if (validate_channel_count(disable_values.size()))
            disable_values_ = disable_values;
    }

    /// Sets the final value of a single channel to be written on disable
    void set_disable_value(uint32 channel_number, T disable_value) {
        if (Module<T>::validate_channel_number(channel_number))
            disable_values_[Module<T>::channel_map_.at(channel_number)] =
                disable_value;
    }

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
    std::vector<T>
        enable_values_;  ///< The initial values set when the Module is enabled
    std::vector<T>
        disable_values_;  ///< The final values set when the Module is disabled
    std::vector<T>
        expire_values_;  ///< The expire values when the Module expires

public:
    /// Encapsulates a Module channel
    class Channel : public ChannelBase<T, Output<T>> {
    public:
        /// Default constructor. Creates invalid channel
        Channel() : ChannelBase<T, Output<T>>() {}

        /// Creates a valid channel.
        Channel(Output* module, uint32 channel_number)
            : ChannelBase<T, Output<T>>(module, channel_number) {}

        /// Sets the enable value of the channel
        void set_enable_value(T enable_value) {
            ChannelBase<T, Output<T>>::module_->set_intial_value(
                ChannelBase<T, Output<T>>::channel_number_, enable_value);
        }

        /// Sets the disable value of the channel
        void set_disable_value(T disable_value) {
            ChannelBase<T, Output<T>>::module_->set_disable_value(
                ChannelBase<T, Output<T>>::channel_number_, disable_value);
        }

        /// Sets the expiration value of the channel
        bool set_expire_value(T expire_value) {
            return ChannelBase<T, Output<T>>::module_->set_expire_value(
                ChannelBase<T, Output<T>>::channel_number_, expire_value);
        }
    };
};

//==============================================================================
// TYPEDEFS
//==============================================================================

typedef Output<Voltage> AnalogOutput;
typedef Output<Logic> DigitalOutput;

}  // namespace mel

#endif  // MEL_OUTPUT_HPP
