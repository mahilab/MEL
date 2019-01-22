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

#include <MEL/Daq/Watchdog.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a hardware watchdog timer
class MEL_API QuanserWatchdog : public Watchdog {
public:
    /// Default constructor
    QuanserWatchdog(QuanserDaq& daq, Time timeout);

    /// Default destructor. Stops the watchdog if watching
    ~QuanserWatchdog();

    bool start() override;

    bool kick() override;

    bool stop() override;

    bool is_expired() override;

    bool clear() override;

private:
    QuanserDaq& daq_;  ///< Reference to parent QDaq
};

}  // namespace mel
