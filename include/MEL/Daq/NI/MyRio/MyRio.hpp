// MIT License
//
// MEL - Mechatronics Engine and Library
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

#include <MEL/Daq/NI/MyRio/MyRioAnalogInput.hpp>

namespace mel {

class MyRio : public Daq, NonCopyable {
public:

    /// Constructor
    MyRio(const std::string& name, bool open = true);

    /// Default Destructor
    ~MyRio();

    /// Opens the MyRio
    virtual bool open() override;

    /// Closes the MyRio
    virtual bool close() override;

    /// Enables the Q8Usb by sequentially calling the enable() function
    /// on all I/O modules. Consult the documentation for each module for
    /// details on what the enable functions do.
    bool enable() override;

    /// Disables the Q8Usb by sequentially calling the disable() function
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


public:

    MyRioAnalogInput analog_input_C;

private:

    friend class MyRioAnalogInput;

};

}  // namespace mel

#endif  // MEL_MYRIO_HPP
