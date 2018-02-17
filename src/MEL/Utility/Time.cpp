#include <MEL/Utility/Time.hpp>
#include <MEL/Utility/Frequency.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

const Time Time::Zero;

Time::Time() :
    microseconds_(0)
{
}

Time::Time(int64 microseconds) :
    microseconds_(microseconds)
{
}

double Time::as_seconds() const
{
    return microseconds_ / 1000000.0;
}

int32 Time::as_milliseconds() const
{
    return static_cast<int32>(microseconds_ / 1000);
}

int64 Time::as_microseconds() const
{
    return microseconds_;
}


Frequency Time::to_frequency() const {
    return megahertz(1.0 / static_cast<double>(microseconds_));
}


//==============================================================================
// INSTANTIATION FUNCTIONS
//==============================================================================

Time seconds(double amount) {
    return Time(static_cast<int64>(amount * 1000000));
}

Time milliseconds(int32 amount) {
    return Time(static_cast<int64>(amount) * 1000);
}

Time microseconds(int64 amount) {
    return Time(amount);
}

//==============================================================================
// OPERATOR OVERLOADS
//==============================================================================

std::ostream& operator << (std::ostream& os, const Time& t) {
    if (t.as_seconds() >= 1.0)
        os << t.as_seconds() << " s";
    else if (t.as_milliseconds() > 1)
        os << t.as_milliseconds() << " ms";
    else
        os << t.as_microseconds() << " us";
    return os;
}


bool operator ==(Time left, Time right) {
    return left.as_microseconds() == right.as_microseconds();
}

bool operator !=(Time left, Time right) {
    return left.as_microseconds() != right.as_microseconds();
}

bool operator <(Time left, Time right) {
    return left.as_microseconds() < right.as_microseconds();
}

bool operator >(Time left, Time right) {
    return left.as_microseconds() > right.as_microseconds();
}

bool operator <=(Time left, Time right) {
    return left.as_microseconds() <= right.as_microseconds();
}

bool operator >=(Time left, Time right) {
    return left.as_microseconds() >= right.as_microseconds();
}

Time operator -(Time right) {
    return microseconds(-right.as_microseconds());
}

Time operator +(Time left, Time right) {
    return microseconds(left.as_microseconds() + right.as_microseconds());
}

Time& operator +=(Time& left, Time right) {
    return left = left + right;
}

Time operator -(Time left, Time right) {
    return microseconds(left.as_microseconds() - right.as_microseconds());
}

Time& operator -=(Time& left, Time right) {
    return left = left - right;
}

Time operator *(Time left, double right) {
    return seconds(left.as_seconds() * right);
}

Time operator *(Time left, int64 right) {
    return microseconds(left.as_microseconds() * right);
}

Time operator *(double left, Time right) {
    return right * left;
}

Time operator *(int64 left, Time right) {
    return right * left;
}

Time& operator *=(Time& left, double right) {
    return left = left * right;
}

Time& operator *=(Time& left, int64 right) {
    return left = left * right;
}

Time operator /(Time left, double right) {
    return seconds(left.as_seconds() / right);
}

Time operator /(Time left, int64 right) {
    return microseconds(left.as_microseconds() / right);
}

Time& operator /=(Time& left, double right) {
    return left = left / right;
}

Time& operator /=(Time& left, int64 right) {
    return left = left / right;
}

double operator /(Time left, Time right) {
    return left.as_seconds() / right.as_seconds();
}

Time operator %(Time left, Time right) {
    return microseconds(left.as_microseconds() % right.as_microseconds());
}

Time& operator %=(Time& left, Time right) {
    return left = left % right;
}

} // namespace mel

//==============================================================================
// APAPTED FROM: SFML (https://www.sfml-dev.org/)
//==============================================================================
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
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
//==============================================================================
