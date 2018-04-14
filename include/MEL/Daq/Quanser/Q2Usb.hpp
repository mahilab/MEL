// MIT License
//
// MEL - Mechatronics Library
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

#ifndef MEL_Q2USB_HPP
#define MEL_Q2USB_HPP

#include <MEL/Daq/Quanser/QAnalogInput.hpp>
#include <MEL/Daq/Quanser/QAnalogOutput.hpp>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QDigitalInput.hpp>
#include <MEL/Daq/Quanser/QDigitalInputOutput.hpp>
#include <MEL/Daq/Quanser/QDigitalOutput.hpp>
#include <MEL/Daq/Quanser/QEncoder.hpp>
#include <MEL/Daq/Quanser/QOptions.hpp>
#include <MEL/Daq/Quanser/QWatchdog.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser Q2 USB class
class Q2Usb : public QDaq {
public:
    /// Default constructor. Creates Q2 USB with all channels enabled and
    /// default QOptions
    Q2Usb(QOptions options = QOptions(),
          bool open        = true,
          uint32 id        = next_id_);

    /// Default destructor. First calls disable() if the Q2Usb is enabled
    /// then close() if the Q2Usb is open.
    ~Q2Usb();

    /// Opens the Q2Usb and sets options and default expiration states (0.0 V, 0
    /// V TTL)
    bool open() override;

    /// Closes the Q2Usb and clears the watchdog
    bool close() override;

    /// Enables the Q2Usb by sequentially calling the enable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do. This function also stops
    /// and clears the watchdog if it is running or expired, and will open
    /// the Q2Usb if it is not currently open.
    bool enable() override;

    /// Disables the Q2Usb by sequentially calling the disable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do. This function also stops
    /// and clears the watchdog if it is running or expired.
    bool disable() override;

    /// Updates all Input Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_input() override;

    /// Updates all Output Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_output() override;

    /// If QOptions::LedMode is User, this function sets the status of the
    /// LED and will be updated on update_output()
    void set_led(Logic value);

    /// Checks for digital loopback between the input and output channels and
    /// returns true if a connection is found. Usefuly for identifying multiple
    /// Q2USBs
    bool identify(uint32 input_channel_number, uint32 output_channel_number);

public:
    /// Determines how many Q2 USBs are currently connected to host.
    /// Only call this function before enabling any Q2 USBs
    static std::size_t get_q2_usb_count();

public:
    QAnalogInput analog_input;       ///< The analog input Module
    QAnalogOutput analog_output;     ///< The analog output Module
    QDigitalInputOutput digital_io;  ///< The digital input/output module
    QEncoder encoder;                ///< The encoder Module
    QWatchdog watchdog;              ///< The watchdog timer of this Q2 USB

private:
    static uint32
        next_id_;  ///< Static counter that determines the next ID# to use
};

}  // namespace mel

#endif  // MEL_Q2USB_HPP

/// http://quanser-update.azurewebsites.net/quarc/documentation/Q2_usb.html
