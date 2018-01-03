#pragma once

#include <MEL/Utility/Types.hpp>

namespace mel
{

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Represents a time value.
class Time {

public:

    /// Default constructor. Sets time value to zero. To construct valued time
    /// objects, use mel::seconds, mel::milliseconds or mel::microseconds.
    Time();

    /// Return the time value as a number of seconds.
    double as_seconds() const;

    /// Return the time value as a number of milliseconds.
    int32 as_milliseconds() const;

    /// Return the time value as a number of microseconds.
    int64 as_microseconds() const;

    static const Time Zero; ///< Predefined "zero" time value

private:

    friend Time seconds(double);
    friend Time milliseconds(int32);
    friend Time microseconds(int64);

    /// Internal constructor from a number of microseconds.
    explicit Time(int64 microseconds);

    int64 microseconds_; ///< Time value stored as microseconds

};

//==============================================================================
// INSTANTIATION FUNCTIONS
//==============================================================================

/// Construct a time value from a number of seconds
Time seconds(double amount);

/// Construct a time value from a number of milliseconds
Time milliseconds(int32 amount);

/// Construct a time value from a number of microseconds
Time microseconds(int64 amount);

//==============================================================================
// OPERATOR OVERLOADS
//==============================================================================

/// Overload of == operator to compare if two time values are equal
bool operator ==(Time left, Time right);

/// Overload of != operator to compare if two time values are not equal
bool operator !=(Time left, Time right);

/// Overload of < operator to compare if left time is less than right
bool operator <(Time left, Time right);

/// Overload of > operator to compare if right time is greater than right
bool operator >(Time left, Time right);

/// Overload of <= operator to compare if left time is less or equal than right
bool operator <=(Time left, Time right);

/// Overload of >= operator to compare if left time is greater or equal than right
bool operator >=(Time left, Time right);

/// Overload of unary - operator to negate a time value
Time operator -(Time right);

/// Overload of binary + operator to add two time values
Time operator +(Time left, Time right);

/// Overload of binary += operator to add/assign two time values
Time& operator +=(Time& left, Time right);

/// Overload of binary - operator to subtract two time values
Time operator -(Time left, Time right);

/// Overload of binary -= operator to subtract/assign two time values
Time& operator -=(Time& left, Time right);

/// Overload of binary * operator to scale a time value
Time operator *(Time left, double right);

/// Overload of binary * operator to scale a time value
Time operator *(Time left, int64 right);

/// Overload of binary * operator to scale a time value
Time operator *(double left, Time right);

/// Overload of binary * operator to scale a time value
Time operator *(int64 left, Time right);

/// Overload of binary *= operator to scale/assign a time value
Time& operator *=(Time& left, double right);

/// Overload of binary *= operator to scale/assign a time value
Time& operator *=(Time& left, int64 right);

/// Overload of binary / operator to scale a time value
Time operator /(Time left, double right);

/// Overload of binary / operator to scale a time value
Time operator /(Time left, int64 right);

/// Overload of binary /= operator to scale/assign a time value
Time& operator /=(Time& left, double right);

/// Overload of binary /= operator to scale/assign a time value
Time& operator /=(Time& left, int64 right);

/// Overload of binary / operator to compute the ratio of two time values
double operator /(Time left, Time right);

/// Overload of binary % operator to compute remainder of a time value
Time operator %(Time left, Time right);

/// Overload of binary %= operator to compute/assign remainder of a time value
Time& operator %=(Time& left, Time right);

} // namespace mel

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
/// \see mel::Clock, mel::Timer, mel::seconds, mel::milliseconds, mel::microseconds

