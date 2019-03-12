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

#include <MEL/Core/Clock.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Limiter {
public:
    /// Default constructor. No limits imposed.
    Limiter();

    /// Limiter in Saturate mode that saturates value based on a absolute limit
    Limiter(double abs_limit);

    /// Limiter in Saturate mode that saturates values based on separate min and
    /// max limits
    Limiter(double min_limit, double max_limit);

    /// Limiter in Accumulate mode that uses an accumulation algorithm (aka
    /// i^2*t)
    Limiter(double continuous_limit, double peak_limit, Time time_limit);

    /// Limits the unlimited value using the Limiter mode
    double limit(double unlimited_value);

    /// Returns true if previous value passed limit() tripped the Limiter
    bool limit_exceeded() const;

    /// Gets the limited value since the last call to limit()
    double get_limited_value() const;

    /// Gets the Limiter setpoint (Accumulate mode only)
    double get_setpoint() const;

    /// Gets the Limiter accumulator (Accumulate mode only)
    double get_accumulator() const;

    /// Resets the Limiter accumulator and clock (Accumulate mode only)
    void reset();

private:
    /// Represents limitation modes
    enum Mode {
        None,       ///< no limits
        Saturate,   ///< limit using saturation
        Accumulate  ///< limit using i^2*t algorithm
    };

    Mode mode_;  ///< limitation mode
    double
        min_limit_;  ///< minimum value allowed in Saturate and Accumulate modes
    double
        max_limit_;  ///< maximum value allowed in Saturate and Accumulate modes
    double continuous_limit_;  ///< maximum continuous value allowed in
                               ///< Accumulate mode
    double setpoint_;  ///< value to be compared against accumulator to regulate
                       ///< switching between continuous and extreme limits in
                       ///< Accumulate mode
    double accumulator_;  ///< value storing the internal memory of the limit in
                          ///< Accumulate mode
    double limited_value_;  ///< modified value after applying limits
    Clock clock_;           ///< internal clock for regulating Accumulate mode
    bool exceeded_;         ///< is true when any limit is exceeded
};

}  // namespace mel