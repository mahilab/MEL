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

#ifndef MEL_MYRIO_HPP
#define MEL_MYRIO_HPP

#include <MEL/Daq/Daq.hpp>
#include <MEL/Utility/NonCopyable.hpp>

#include <MEL/Daq/NI/MyRio/MyRioAI.hpp>
#include <MEL/Daq/NI/MyRio/MyRioAO.hpp>
#include <MEL/Daq/NI/MyRio/MyRioDIO.hpp>

namespace mel {

class MyRioAI;
class MyRioAO;

/// myRIO Expansion Port (MXP) and Mini System Port (MSP) connector types
enum MyRioConnectorType : int {
    MxpA  = 0,  ///< MXP connector A (rear)
    MxpB  = 1,  ///< MXP connector B (rear)
    MspC  = 2,  ///< MSP connector C (front)
    Audio = 3   ///< Audio connector (front)
};

/// National Instruments myRIO embedded system
class MyRio : public Daq, NonCopyable {

public:

    /// Constructor
    MyRio(const std::string& name, bool auto_open = true);

    /// Default Destructor
    ~MyRio();

    /// Opens the myRIO
    virtual bool open() override;

    /// Closes the myRIO
    virtual bool close() override;

    /// Enables the myRIO by sequentially calling the enable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do.
    bool enable() override;

    /// Disables the myRIO by sequentially calling the disable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do.
    bool disable() override;

    /// Updates all Input Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_input() override;

    /// Updates all Output Modules simultaneously. It is generally more
    /// efficient to call this once per loop, than to call the update()
    /// function on each module separately.
    bool update_output() override;

private:

    /// Represents a myRIO connector
    struct Connector : public Device {
        Connector(MyRio& myrio, MyRioConnectorType type,
            const std::vector<uint32>& ai_channels,
            const std::vector<uint32>& ao_channels,
            const std::vector<uint32>& dio_channels);
        bool enable() override;
        bool disable() override;
        bool update_input();
        bool update_output();
        MyRioAI AI;
        MyRioAO AO;
        MyRioDIO DIO;
    };

public:

     Connector A;  ///< MXP connector A
     Connector B;  ///< MXP connector B
     Connector C;  ///< MSP connector C

};

}  // namespace mel

#endif  // MEL_MYRIO_HPP
