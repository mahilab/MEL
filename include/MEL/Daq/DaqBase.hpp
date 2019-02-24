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


#include <MEL/Core/Device.hpp>
#include <MEL/Daq/Module.hpp>
#include <typeindex>
#include <unordered_map>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class DaqBase : public Device {
public:

    /// Constructor
    DaqBase(const std::string& name);

    /// Destructor. By default, this closes the DAQ.
    virtual ~DaqBase();

    /// Opens communication with DAQ.
    ///
    /// \return TRUE if open successful, FALSE otherwise
    bool open();

    /// Closes communication with DAQ.
    ///
    /// \return TRUE if open successful, FALSE otherwise
    bool close();

    /// Updates all Input modules contained on the DAQ
    ///
    /// \return TRUE if successful, FALSE otherwise
    virtual bool update_input();

    /// Updates all Output modules contained on the DAQ
    ///
    /// \return TRUE if successful, FALSE otherwise
    virtual bool update_output();

    /// Returns whether the DAQ is open or closed
    ///
    /// \return TRUE if open, FALSE if closed
    bool is_open() const;

protected:

    /// Implement this function to open communication with your DAQ
    ///
    /// \return TRUE if open successful, FALSE otherwise
    virtual bool on_open() = 0;

    /// Implement this function to close communication with your DAQ
    ///
    /// \return TRUE if close successful, FALSE otherwise
    virtual bool on_close() = 0;

private:

    bool open_;  ///< TRUE if open, FALSE if closed

};

}  // namespace mel
