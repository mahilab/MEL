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
#include <MEL/Daq/NI/MyRio/MyRioConnector.hpp>

namespace mel {

/// National Instruments myRIO embedded system
class MyRio : public DaqBase, NonCopyable {

public:

    /// Constructor
    MyRio();

    /// Default Destructor
    ~MyRio();

    /// Resets the myRIO FPGA to default configuration
    bool reset();

    /// Updates all connector inputs simultaneously.
    bool update_input() override;

    /// Updates all connector outputs simultaneously.
    bool update_output() override;

    /// Returns true if the myRIO button is currently pressed
    bool is_button_pressed() const;

    /// Set myRIO led in range 0 to 3 on/off
    void set_led(int led, bool on);

private:

    bool on_open() override;
    bool on_close() override;
    bool on_enable() override;
    bool on_disable() override;

public:

     MyRioMxp mxpA;  ///< MXP connector A
     MyRioMxp mxpB;  ///< MXP connector B
     MyRioMsp mspC;  ///< MSP connector C

};

}  // namespace mel
