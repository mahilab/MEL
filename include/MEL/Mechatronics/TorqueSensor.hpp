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

#include <MEL/Core/Types.hpp>
#include <vector>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class TorqueSensor {
public:

    /// Constructor
    TorqueSensor();

    /// Destructor
    virtual ~TorqueSensor();

    /// Returns torque along speficied axis
    virtual double get_torque(Axis axis) = 0;

    /// Returns torques along X, Z, and Z axes
    virtual std::vector<double> get_torques() = 0;

protected:

    std::vector<double> torques_; ///< measured torques

};

}  // namespace mel
