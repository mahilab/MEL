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
// This particular source file includes code which has been adapted from the
// following open-source projects (all external licenses attached at bottom):
//     SFML - Simple and Fast Multimedia Library
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_TIME_HPP
#define MEL_TIME_HPP

#include <MEL/Config.hpp>
#include <MEL/Utility/Types.hpp>
#include <iostream>

namespace mel {

class Frequency;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Represents a time value.
class MEL_API Time {
public:
    /// Default constructor. Sets time value to zero. To construct valued time
    /// objects, use mel::seconds, mel::milliseconds or mel::microseconds.
    Time();

    /// Overloads stream operator
    friend MEL_API std::ostream& operator<<(std::ostream& os, const Time& t);

    /// Return the time value as a number of seconds.
    double as_seconds() const;

    /// Return the time value as a number of milliseconds.
    int32 as_milliseconds() const;

    /// Return the time value as a number of microseconds.
    int64 as_microseconds() const;

    /// Returns the reciprocal time as a Frequency
    Frequency to_frequency() const;

public:
    static const Time Zero;  ///< Predefined "zero" time value
    static const Time Inf;   ///< Predefined "infinite time value

private:
    friend MEL_API Time seconds(double);
    friend MEL_API Time milliseconds(int32);
    friend MEL_API Time microseconds(int64);

    /// Internal constructor from a number of microseconds.
    explicit Time(int64 microseconds);

private:
    int64 microseconds_;  ///< Time value stored as microseconds
};

//==============================================================================
// INSTANTIATION FUNCTIONS
//==============================================================================

/// Construct a time value from a number of seconds
MEL_API Time seconds(double amount);

/// Construct a time value from a number of milliseconds
MEL_API Time milliseconds(int32 amount);

/// Construct a time value from a number of microseconds
MEL_API Time microseconds(int64 amount);

//==============================================================================
// OPERATOR OVERLOADS
//==============================================================================

/// Overload of << stream operator
MEL_API std::ostream& operator<<(std::ostream& os, const Time& t);

/// Overload of == operator to compare if two time values are equal
MEL_API bool operator==(Time left, Time right);

/// Overload of != operator to compare if two time values are not equal
MEL_API bool operator!=(Time left, Time right);

/// Overload of < operator to compare if left time is less than right
MEL_API bool operator<(Time left, Time right);

/// Overload of > operator to compare if right time is greater than right
MEL_API bool operator>(Time left, Time right);

/// Overload of <= operator to compare if left time is less or equal than right
MEL_API bool operator<=(Time left, Time right);

/// Overload of >= operator to compare if left time is greater or equal than
/// right
MEL_API bool operator>=(Time left, Time right);

/// Overload of unary - operator to negate a time value
MEL_API Time operator-(Time right);

/// Overload of binary + operator to add two time values
MEL_API Time operator+(Time left, Time right);

/// Overload of binary += operator to add/assign two time values
MEL_API Time& operator+=(Time& left, Time right);

/// Overload of binary - operator to subtract two time values
MEL_API Time operator-(Time left, Time right);

/// Overload of binary -= operator to subtract/assign two time values
MEL_API Time& operator-=(Time& left, Time right);

/// Overload of binary * operator to scale a time value
MEL_API Time operator*(Time left, double right);

/// Overload of binary * operator to scale a time value
MEL_API Time operator*(Time left, int64 right);

/// Overload of binary * operator to scale a time value
MEL_API Time operator*(double left, Time right);

/// Overload of binary * operator to scale a time value
MEL_API Time operator*(int64 left, Time right);

/// Overload of binary *= operator to scale/assign a time value
MEL_API Time& operator*=(Time& left, double right);

/// Overload of binary *= operator to scale/assign a time value
MEL_API Time& operator*=(Time& left, int64 right);

/// Overload of binary / operator to scale a time value
MEL_API Time operator/(Time left, double right);

/// Overload of binary / operator to scale a time value
MEL_API Time operator/(Time left, int64 right);

/// Overload of binary /= operator to scale/assign a time value
MEL_API Time& operator/=(Time& left, double right);

/// Overload of binary /= operator to scale/assign a time value
MEL_API Time& operator/=(Time& left, int64 right);

/// Overload of binary / operator to compute the ratio of two time values
MEL_API double operator/(Time left, Time right);

/// Overload of binary % operator to compute remainder of a time value
MEL_API Time operator%(Time left, Time right);

/// Overload of binary %= operator to compute/assign remainder of a time value
MEL_API Time& operator%=(Time& left, Time right);

}  // namespace mel

#endif  // MEL_TIME_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::Time
/// \ingroup system
///
/// mel::Time encapsulates a time value in a flexible way.
/// It allows to define a time value either as a number of
/// seconds, milliseconds or microseconds. It also works the
/// other way round: you can read a time value as either
/// a number of seconds, milliseconds or microseconds.
///
/// By using such a flexible interface, the API doesn't
/// impose any fixed type or resolution for time values,
/// and let the user choose its own favorite representation.
///
/// Time values support the usual mathematical operations:
/// you can add or subtract two times, multiply or divide
/// a time by a number, compare two times, etc.
///
/// Since they represent a time span and not an absolute time
/// value, times can also be negative.
///
/// Usage example:
/// \code
/// mel::Time t1 = mel::seconds(0.1f);
/// int32 milli = t1.as_milliseconds(); // 100
///
/// mel::Time t2 = mel::milliseconds(30);
/// int64 micro = t2.as_microseconds(); // 30000
///
/// mel::Time t3 = mel::microseconds(-800000);
/// double sec = t3.as_seconds(); // -0.8
/// \endcode
///
/// \code
/// void update(mel::Time elapsed)
/// {
///    position += speed * elapsed.as_seconds();
/// }
///
/// update(mel::milliseconds(100));
/// \endcode
///
/// \see mel::Clock, mel::Timer, mel::seconds, mel::milliseconds,
/// mel::microseconds

//==============================================================================
// LICENSES
//==============================================================================

// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
