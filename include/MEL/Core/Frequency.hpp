// MIT License
//
// MEL - Mechatronics Library
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

#ifndef MEL_FREQUENCY_HPP
#define MEL_FREQUENCY_HPP

#include <MEL/Utility/Types.hpp>

namespace mel {

class Time;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Represents a frequency value
class Frequency {
public:
    /// Default constructor. Sets frequency value to zero. To construct valued
    /// frequency objects, use hertz(), kilohertz() or megahertz()
    Frequency();

    /// Overloads stream operator
    friend std::ostream& operator<<(std::ostream& os, const Frequency& f);

    /// Return the frequency value as a number of hertz
    int64 as_hertz() const;

    /// Return the frequency value as a number of kilohertz
    int32 as_kilohertz() const;

    /// Return the frequency value as a number of megahertz
    double as_megahertz() const;

    /// Returns the reciprocal frequency as a Time
    Time to_time() const;

public:
    static const Frequency Zero;  ///< Predefined "zero" frequency value
    static const Frequency Inf;  ///< Predfined "infinite" frequency value

private:
    friend Frequency hertz(int64);
    friend Frequency kilohertz(int32);
    friend Frequency megahertz(double);

    /// Internal constructor from a number of hertz
    explicit Frequency(int64 hertz);

private:
    int64 hertz_;  ///< Time value stored as microseconds
};

//==============================================================================
// INSTANTIATION FUNCTIONS
//==============================================================================

/// Construct a time value from a number of seconds
Frequency hertz(int64 amount);

/// Construct a time value from a number of milliseconds
Frequency kilohertz(int32 amount);

/// Construct a time value from a number of microseconds
Frequency megahertz(double amount);

}  // namespace mel

#endif  // MEL_FREQUENCY_HPP
