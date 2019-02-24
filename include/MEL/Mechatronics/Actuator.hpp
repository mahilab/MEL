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
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#pragma once

#include <MEL/Core/Device.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Actuator : public Device {
public:

    /// Default Constructor
    Actuator();

    /// Constructor for actuator without torque limits
    Actuator(const std::string& name);

    /// This function should set the desired torque to be generated at the
    /// Actuator
    virtual void set_torque(double torque) = 0;

    /// This function should return the attempted command current
    virtual double get_torque_command() const;

    /// This function should return the limited command current if limitations
    /// are imposed
    virtual double get_torque_limited() const;

    /// This function should return the torque sense of the actuator if it is
    /// available
    virtual double get_torque_sense() const;

protected:
    mutable double torque_command_;  ///< Stores the Actuator torque since the
                                     ///< last call to set_torque()
};

}  // namespace mel
