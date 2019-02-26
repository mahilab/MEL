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
#include <MEL/Core/Device.hpp>
#include <MEL/Daq/Registry.hpp>
#include <map>

namespace mel {

/// Defines non-templated Module functions/members
class ModuleBase : public Device {
public:

    /// Default Constructor (creates an invlaid empty Module)
    ModuleBase();

    /// Constructor with specified channel numbers
    ModuleBase(const ChanNums& channel_numbers);

    /// Calls the Modules's API to update a single channel with the real-world.
    virtual bool update_channel(ChanNum channel_number) = 0;

    /// Calls the Modules's API to update all channels with the real-world.
    /// By default, iteratively calls update_channel() on all channel numbers.
    virtual bool update();

    /// Gets the vector of channel numbers this Module maintains
    const ChanNums& get_channel_numbers() const;

    /// Returns the number of channels on this Module
    std::size_t get_channel_count() const;

    /// Checks if a channel number is a number defined on this Module.
    /// If quiet is false, an error log will be thrown.
    bool validate_channel_number(ChanNum channel_number, bool quiet = false) const;

    /// Checks if the size of a vector equals the number of channels
    /// If quiet is false, an error log will be thrown.
    bool validate_channel_count(std::size_t size, bool quiet = false) const;

protected:

    /// Sets the channel numbers this Module maintains
    virtual void set_channel_numbers(const ChanNums& channel_numbers);

    /// Adds a channel number to current channel numbers
    virtual void add_channel_number(ChanNum channel_number);

    /// Removes a channel number from current channel numbers
    virtual void remove_channel_number(ChanNum channel_number);

    /// Called when the user enables the Module
    virtual bool on_enable() override;

    /// Called when the user disables the Module
    virtual bool on_disable() override;

private:

    friend class RegistryBase;

    /// Adds a Registry to this Module
    void add_registry(RegistryBase* registry);

    /// Updates the channel Map and notifies Registries
    void update_map();

    /// Returns vector index associated with channel number
    std::size_t index(uint32 channel_number) const;

private:

    ChanNums channel_numbers_;              ///< The channel numbers used by this ModuleBase
    ChanMap  channel_map_;                  ///< Maps a channel number with a vector index position
    std::vector<RegistryBase*> registries_; ///< Registries needed by this Module

};

/// Defines templated Module functions/members
template <typename T>
class Module : public ModuleBase {
public:

    /// Default Constructor (creates an invlaid empty Module)
    Module();

    /// Constructor with specified channel numbers
    Module(const ChanNums& channel_numbers);

    /// Destructor
    virtual ~Module();

    /// Sets the min and max possible values of each channel
    virtual bool set_ranges(const std::vector<T>& min_values, const std::vector<T>& max_values);

    /// Sets the min and max possible value of a single channel
    virtual bool set_range(ChanNum channel_number, T min_value, T max_value);

    /// Gets non-const reference to the current channel values of this Module
    std::vector<T>& get_values();

    /// Gets the current value of a a single channel. If an invalid channel number
    /// is passed, the default value of the underlying channel type is returned.
    T get_value(ChanNum channel_number) const;

    /// Sets the current channel values of this Module. If the incorrect number
    /// of values is pass, no values are set.
    void set_values(const std::vector<T>& values);

    /// Sets the current value of a single channel. If an invalid channel number
    /// is passed, non value is set
    void set_value(ChanNum channel_number, T value);

protected:

    Registry<T> values_;      ///< The real-world values of the channels in this Module
    Registry<T> min_values_;  ///< The minimum possible values of each channel
    Registry<T> max_values_;  ///< The maximum possible values of each channel

};

} // namespace mel

#include <MEL/Daq/Detail/Module.inl>
