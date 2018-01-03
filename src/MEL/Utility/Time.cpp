#include <MEL/Utility/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

const Time Time::Zero;

Time::Time() :
    microseconds_(0)
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

Time::Time(int64 microseconds) :
    microseconds_(microseconds)
{
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
