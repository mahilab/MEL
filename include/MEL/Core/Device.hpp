//==============================================================================
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
//            Craig McDonald (craig.g.mcdonald@gmail.com)
//==============================================================================

#pragma once

#include <string>

namespace mel {

//==============================================================================
/// Abstract base class from which all physical devices derive.
//==============================================================================
class Device {
public:

    //==========================================================================
    /// Default constructor.
    //==========================================================================
    Device();

    //==========================================================================
    /// Constructs a Device with a specific string name
    //==========================================================================
    Device(const std::string& name);

    //==========================================================================
    /// Default destructor.
    //==========================================================================
    virtual ~Device();

    //==========================================================================
    /// Enables the Device and returns TRUE if successful, FALSE otherwise
    //==========================================================================
    bool enable();

    //==========================================================================
    /// Disables the Device and returns TRUE if successful, FALSE otherwise
    //==========================================================================
    bool disable();

    //==========================================================================
    /// Returns true if the Device is enabled or false if it is disabled
    //==========================================================================
    bool is_enabled() const;

    //==========================================================================
    /// Gets the Device's string name
    //==========================================================================
    const std::string& get_name() const;

    //==========================================================================
    /// Sets the Device's string name
    //==========================================================================
    void set_name(const std::string& name);

protected:

    //==========================================================================
    /// Implement this function with code that should be called when enabled
    ///
    /// \return TRUE if successful, FALSE otherwise
    //==========================================================================
    virtual bool on_enable() = 0;

    //==========================================================================
    /// Implement this function with code that should be called when disabled
    ///
    /// \return TRUE if successful, FALSE otherwise
    //==========================================================================
    virtual bool on_disable() = 0;

private:
    bool enabled_;      ///< The Device enabled status
    std::string name_;  ///< The Device name
};

}  // namespace mel
