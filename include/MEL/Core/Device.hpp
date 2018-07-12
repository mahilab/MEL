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
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_DEVICE_HPP
#define MEL_DEVICE_HPP

#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Abstract base class from which all physical devices should derive.
class Device {
public:
    /// Default constructor
    Device();

    /// Prefered constructor. Every device in MEL should have a unique name.
    Device(const std::string& name);

    /// Default destructor.
    virtual ~Device();

    /// The function should enable the device, performing start up and
    /// initialization procedures. It should return true if successful,
    /// false otherwise, and set #enabled_ accordingly
    virtual bool enable();

    /// The function should disable the device, performing necessary shutdown
    /// procedures. It should  return true if successful, false otherwise,
    /// and set #enabled_ accordingly
    virtual bool disable();

public:
    /// Returns true if the device is enabled, false if disabled.
    bool is_enabled() const;

    /// Returns the Device's name
    const std::string& get_name() const;

    /// Returns the Device's long name which includes its class lineage
    const std::string& get_long_name() const;

    /// Sets the Device's string name
    void set_name(const std::string& name);

private:
    bool enabled_;           ///< The Device enabled status
    std::string long_name_;  ///< the Device long name
    std::string name_;       ///< The Device short name
};

}  // namespace mel

#endif  // MEL_DEVICE_HPP
