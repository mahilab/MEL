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

#ifndef MEL_MODULE_HPP
#define MEL_MODULE_HPP

#include <MEL/Core/Device.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Types.hpp>
#include <algorithm>
#include <map>
#include <vector>

namespace mel {

//==============================================================================
// TYPES
//==============================================================================

/// The Module/Channel type
enum class IoType {
    InputOnly,   ///< Module that reads in real-world values
    OutputOnly,  ///< Module that writes out real-world values
    InputOutput  ///< Module that reads or writes real-world values
};

//==============================================================================
// BASE DECLARATION
//==============================================================================

/// Defines non-templated Module functions/members
class ModuleBase : public Device {

public:

    /// Constructor
    ModuleBase(const std::string& name, IoType type, const std::vector<uint32>& channel_numbers);

    /// Calls the Modules's API to update all channels with the real-world.
    virtual bool update() = 0;

    /// Calls the Modules's API to update a single channel with the real-world.
    virtual bool update_channel(uint32 channel_number) = 0;

    /// Gets the vector of channel numbers this Module maintains
    const std::vector<uint32>& get_channel_numbers() const;

    /// Returns the number of channels on this Module
    std::size_t get_channel_count() const;

    /// Returns the IoType of this Module
    IoType get_type() const;

    /// Checks if a channel number is a number defined on this Module
    bool validate_channel_number(uint32 channel_number) const;

    /// Checks if the size of a vector equals the number of channels
    bool validate_channel_count(std::size_t size) const;

protected:

    /// Sorts and removes duplicates from a vector of channel numbers
    static std::vector<uint32> sort_and_reduce_channels(const std::vector<uint32>& channels);

    /// Makes an associative channel map mapping channel number to vector index
    static std::map<uint32, std::size_t> make_channel_map(const std::vector<uint32>& channel_numbers);

protected:

    const IoType type_ ;                               ///< The IoType of this ModuleBase
    const std::vector<uint32> channel_numbers_;        ///< The channel numbers used by this ModuleNase
    const std::size_t channel_count_;                  ///< The total number of channels used by this ModuleBase
    const std::map<uint32, std::size_t> channel_map_;  ///< Associates a channel number with a vector index position

};

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Defines templated Module functions/members
template <typename T>
class Module : public ModuleBase {

public:

    /// Constructor
    Module(const std::string& name, IoType type, const std::vector<uint32>& channel_numbers) :
        ModuleBase(name, type, channel_numbers),
        values_(channel_count_),
        min_values_(channel_count_),
        max_values_(channel_count_)
    {
    }

    /// Destructor
    virtual ~Module() { }

    /// Sets the min and max possible values of each channel
    virtual bool set_ranges(const std::vector<T>& min_values, const std::vector<T>& max_values) {
        if (validate_channel_count(min_values.size()) && validate_channel_count(max_values.size())) {
            min_values_ = min_values;
            max_values_ = max_values;
            return true;
        }
        return false;
    }

    /// Sets the min and max possible value of a single channel
    virtual bool set_range(uint32 channel_number, T min_value, T max_value) {
        if (validate_channel_number(channel_number)) {
            min_values_[channel_map_.at(channel_number)] = min_value;
            max_values_[channel_map_.at(channel_number)] = max_value;
            return true;
        }
        return false;
    }

public:

    /// Gets non-const reference to the current channel values of this Module
    std::vector<T>& get_values() {
        return values_;
    }

    /// Gets the current value of a a single channel. If an invalid channel number
    /// is passed, the default value of the underlying channel type is returned.
    T get_value(uint32 channel_number) const {
        if(validate_channel_number(channel_number))
            return values_[channel_map_.at(channel_number)];
        return T();
    }

    /// Sets the current channel values of this Module. If the incorrect number
    /// of values is pass, no values are set.
    void set_values(const std::vector<T>& values) {
        if (validate_channel_count(values.size()))
            values_ = values;
    }

    /// Sets the current value of a single channel. If an invalid channel number
    /// is passed, non value is set
    void set_value(uint32 channel_number, T value) {
        if (validate_channel_number(channel_number))
            values_[channel_map_.at(channel_number)] = value;
    }

protected:

    std::vector<T> values_;          ///< The real-world values of the channels in this Module
    std::vector<T> min_values_;      ///< The minimum possible values of each channel
    std::vector<T> max_values_;      ///< The maximum possible values of each channel

};

} // namespace mel

#endif // MEL_MODULE_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
