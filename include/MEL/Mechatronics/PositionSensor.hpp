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

class PositionSensor {
public:
    /// Constructor
    PositionSensor();

    /// Destructor
    virtual ~PositionSensor();

    /// This function should return the position of the PositionSensor
    virtual double get_position() = 0;

protected:
    double position_;  ///< stores the PositionSensor position since the last
                       ///< call to get_position()
};

}  // namespace mel
