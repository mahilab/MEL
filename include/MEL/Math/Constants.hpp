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

#ifndef MEL_CONSTANTS_HPP
#define MEL_CONSTANTS_HPP

#include <MEL/Config.hpp>

namespace mel {

//==============================================================================
// GENERIC
//==============================================================================

extern const double MEL_API PI;    ///< pi
extern const double MEL_API E;     ///< e
extern const double MEL_API SQRT2; /// sqrt(2)
extern const double MEL_API EPS;   ///< smallest double such that 1.0 + EPS != 1.0
extern const double MEL_API INF;   ///< positive infinity (negate for negative infinity)
extern const double MEL_API NaN;   ///< not-a-number

//==============================================================================
// CONVERSIONS
//==============================================================================

extern const double MEL_API DEG2RAD;     ///< multiply degrees by this value to convert to radians
extern const double MEL_API RAD2DEG;     ///< multiply radians by this value to convert to degrees
extern const double MEL_API INCH2METER;  ///< multiply inches by this value to convert to meters
extern const double MEL_API METER2INCH;  ///< multiply meters by this value to convert to inches

}  // namespace mel

#endif  // MEL_CONSTANTS_HPP
