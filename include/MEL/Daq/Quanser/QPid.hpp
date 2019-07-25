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
#include <MEL/Daq/Quanser/QuanserDaq.hpp>
#include <MEL/Daq/Quanser/QuanserAI.hpp>
#include <MEL/Daq/Quanser/QuanserAO.hpp>
#include <MEL/Daq/Quanser/QuanserDIO.hpp>
#include <MEL/Daq/Quanser/QuanserEncoder.hpp>
#include <MEL/Daq/Quanser/QuanserOptions.hpp>
#include <MEL/Daq/Quanser/QuanserWatchdog.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser Q8 USB class
class QPid : public QuanserDaq {
public:
    /// Default constructor
    QPid(QuanserOptions options = QuanserOptions(),
          uint32 id             = next_id());

    /// Default destructor. First calls disable() if the QPid is enabled
    /// then close() if the QPid is open.
    ~QPid();

    /// Updates all Input Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_input() override;

    /// Updates all Output Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_output() override;

public:
    QuanserAI      AI;         ///< The analog input Module
    QuanserAO      AO;         ///< The analog output Module
    QuanserDIO     DIO;         ///< The digital input Module
    QuanserEncoder  encoder;   ///< The encoder Module
    QuanserWatchdog watchdog;  ///< The watchdog timer of this Q8 USB

private:

    bool on_open() override;
    bool on_close() override;

    /// Enables the QPid by sequentially calling the enable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do.
    bool on_enable() override;

    /// Disables the QPid by sequentially calling the disable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do.
    bool on_disable() override;

private:
    /// Returns the next automatic ID# to use
    static uint32 next_id();
};

}  // namespace mel

/// http://quanser-update.azurewebsites.net/quarc/documentation/q8_usb.html
