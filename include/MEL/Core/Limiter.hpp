// MIT License
//
// MEL - MAHI Exoskeleton Library
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

#ifndef MEL_LIMITER_HPP
#define MEL_LIMITER_HPP

#include <MEL/Utility/Clock.hpp>

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

    /// Limiter in Saturate mode that saturates values based on separate min and max limits
    Limiter(double min_limit, double max_limit);

    /// Limiter in Accumulate mode that uses an accumulation algorithm (aka i^2*t)
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

    Mode mode_;
    double min_limit_;
    double max_limit_;
    double continouous_limit_;
    double setpoint_;
    double accumulator_;
    double limited_value_;
    Clock clock_;
    bool exceeded_;

};

} // namespace mel

#endif // MEL_LIMITER_HPP
