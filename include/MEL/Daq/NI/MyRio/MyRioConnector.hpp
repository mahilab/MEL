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
#include <MEL/Daq/DaqBase.hpp>
#include <MEL/Core/NonCopyable.hpp>

#include <MEL/Daq/NI/MyRio/MyRioAI.hpp>
#include <MEL/Daq/NI/MyRio/MyRioAO.hpp>
#include <MEL/Daq/NI/MyRio/MyRioDIO.hpp>
#include <MEL/Daq/NI/MyRio/MyRioEncoder.hpp>

namespace mel {

class MyRio;

/// myRIo Connector
class MyRioConnector : public DaqBase, NonCopyable {
public:

    /// Types of myRIO connector
    enum Type {
        MxpA = 0,
        MxpB = 1,
        MspC = 2
    };

    /// Updates all Input Modules simultaenously
    bool update_input();

    /// Updates all Output Modules simultaenously
    bool update_output();

    /// Resets the connector digital pin configurations
    void reset();

public:

    const Type type;      ///< connector Type
    MyRioAI AI;           ///< connector analog input
    MyRioAO AO;           ///< connector analog output
    MyRioDIO DIO;         ///< connector digital input/output
    MyRioEncoder encoder; ///< connector encoder

private:

    friend class MyRio;

    /// Private constructor
    MyRioConnector(MyRio& myrio, Type type);

    // Hide open/close functins
    using DaqBase::open;
    using DaqBase::close;

    // Implement virtual functions
    bool on_open() override;
    bool on_close() override;
    bool on_enable() override;
    bool on_disable() override;

private:

    MyRio& myrio_;  ///< reference to myRIO this connector is on

};

} // namespace mel

// MXP Shared Pins
// I2C[0] = DIO[14:15]
// ENC[0] = DIO[11:12]
// PWM[0] = DIO[8]
// PWM[1] = DIO[9]
// PWM[2] = DIO[10]
// SPI[0] = DIO[5:7]

// MSP Shared Pins
// PWM[0] = DIO[3]
// PWM[1] = DIO[7]
// ENC[0] = DIO[0,2]
// ENC[1] = DIO[4,6]