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

#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a hardware watchdog timer
class Watchdog : NonCopyable {
public:
    /// Default constructor
    Watchdog(Time timeout);

    /// Default destructor
    virtual ~Watchdog();

    /// This function should call the DAQ's API to start the watchdog timer on
    /// the #timeout_ period and return true if successful, false otherwise.
    virtual bool start() = 0;

    /// This function should call the DAQ's API to reload the watchdog timer, or
    /// "kick the dog". It should return true if successful, false otherwise. If
    /// the watchdog has expired, it should return false. If the watchdog hasn't
    /// been started, it should implicitly do so.
    virtual bool kick() = 0;

    /// This function should call the DAQ's API to stop and clear the watchdog
    /// timer if it supports one and return true if successful, false otherwise.
    virtual bool stop() = 0;

    /// This function should call the DAQ's API to determine if this watchdog
    /// timer is in an expired state.
    virtual bool is_expired() = 0;

    /// This function should call the DAQ's API to reset the watchdog if it has
    /// previously entered an expired state.
    virtual bool clear() = 0;

public:
    /// Sets the timeout period this Watchdog should operate on
    void set_timeout(Time timeout);

protected:
    Time timeout_;   ///< The timeout period for this Watchdog
    bool watching_;  ///< True if watchdog has been started, false if stopped
};

}  // namespace mel
