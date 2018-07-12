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

#ifndef MEL_COMPONENT_HPP
#define MEL_COMPONENT_HPP

#include <MEL/Experimental/Engine/Static/Object.hpp>

namespace mel {

/// Component Interface
class Component {
public:

    /// Default Constructor
    Component() {}

    /// Virtual Destructor
    virtual ~Component();

    /// Called when this Component's Object starts running
    virtual void start();

    /// Called every time this Component's Object updates
    virtual void update();

    /// Called every time this Component's Object late updates
    virtual void late_update();

    /// Called this Component's Object stops running
    virtual void stop();

    /// Called when this Component's Object resets
    virtual void reset();

private:

    void set_references() {

    }

};

} // namespace mel

#endif // MEL_COMPONENT_HPP
