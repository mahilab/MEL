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
#include <MEL/Daq/Quanser/QuanserDI.hpp>
#include <MEL/Daq/Quanser/QuanserDO.hpp>
#include <MEL/Daq/Quanser/QuanserEncoder.hpp>
#include <MEL/Daq/Quanser/QuanserOptions.hpp>
#include <MEL/Daq/Quanser/QuanserWatchdog.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser Q8 USB class
class Q8Usb : public QuanserDaq {
public:
    /// Default constructor
    Q8Usb(QuanserOptions options                  = QuanserOptions(),
          bool perform_sanity_check               = true,
          uint32 id                               = next_id());

    /// Default destructor. First calls disable() if the Q8Usb is enabled
    /// then close() if the Q8Usb is open.
    ~Q8Usb();


    /// Updates all Input Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_input() override;

    /// Updates all Output Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_output() override;

    /// Quanser provides no software-based method of differntiating Q8 USBs
    /// when more than one is connected to the host. This funcion provides a
    /// physical means by checking for a digital loopback on same numbered
    /// digital input and output channels. For example, if you connect two Q8
    /// USBs to the host, one which has DO5 connected to DI5 (arbitraily
    /// chosen), and the MEL Q8Usb object with id = 0 returns identify(5) =
    /// true, you know Q8Usb(id = 0) is the Q8 USB with the loopback, and
    /// Q8Usb(id = 1) is the Q8 USB without a loopback. If identify(5) = false,
    /// you know Q8Usb(id = 0) is the Q8 USB without the loopback, and Q8Usb(id
    /// = 1) can be infered as the Q8 USB with a loopback, and subsequently
    /// checked by calling indentify(5) on it.
    bool identify(uint32 channel_number);

    /// This is an alternate version of identify() that checks for a loopback
    /// connection on all digital channel pairs, and returns the channel number
    /// of the first connection found. If no connection is found, it returns -1.
    int identify();

public:
    /// Determines how many Q8 USBs are currently connected to host.
    /// Only call this function before enabling any Q8 USBs
    static std::size_t get_q8_usb_count();

public:
    QuanserAI      AI;         ///< The analog input Module
    QuanserAO      AO;         ///< The analog output Module
    QuanserDI      DI;         ///< The digital input Module
    QuanserDO      DO;         ///< The digital output Module
    QuanserEncoder  encoder;   ///< The encoder Module
    QuanserWatchdog watchdog;  ///< The watchdog timer of this Q8 USB

private:

    bool on_open() override;
    bool on_close() override;


    /// Enables the Q8Usb by sequentially calling the enable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do.
    bool on_enable() override;

    /// Disables the Q8Usb by sequentially calling the disable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do.
    bool on_disable() override;

    /// Quarc can sometimes fail to properly intialize a Q8 USB even if it says
    /// otherwise. In these cases, velocity readings are wildly incorrect and
    /// can result in catstrophic behavior. This function performs a sanity
    /// check by checking that all velocities are zero on startup if
    /// #perform_sanity_check_ is true (default).
    bool sanity_check();

private:
    bool perform_sanity_check_;  ///< If true, the sanity check will be
                                 ///< performed on enable
private:
    /// Returns the next automatic ID# to use
    static uint32 next_id();
};

}  // namespace mel

/// http://quanser-update.azurewebsites.net/quarc/documentation/q8_usb.html
