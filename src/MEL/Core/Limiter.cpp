#include <MEL/Core/Limiter.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

Limiter::Limiter() :
    mode_(None)
{
}

Limiter::Limiter(double abs_limit) :
    mode_(Saturate),
    min_limit_(-abs(abs_limit)),
    max_limit_(abs(abs_limit))
{
}

Limiter::Limiter(double min_limit, double max_limit) :
    mode_(Saturate),
    min_limit_(min_limit),
    max_limit_(max_limit)
{
}

Limiter::Limiter(double continuous_limit, double abs_limit, Time time_limit) :
    mode_(Accumulate),
    min_limit_(-abs(abs_limit)),
    max_limit_(abs(abs_limit)),
    continouous_limit_(continuous_limit),
    setpoint_( (sq(abs_limit) - sq(continuous_limit)) * time_limit.as_seconds() ),
    accumulator_(0.0),
    limited_value_(0.0),
    clock_(Clock())
{
}

double Limiter::limit(double unlimited_value) {
    switch(mode_) {
        case None:
            limited_value_ = unlimited_value;
            break;
        case Saturate:
            limited_value_ = saturate(unlimited_value, min_limit_, max_limit_);
            break;
        case Accumulate:
            accumulator_ += (sq(limited_value_) - sq(continouous_limit_)) * clock_.get_elapsed_time().as_seconds();
            accumulator_ = saturate(accumulator_, 0.0, INF);
            clock_.restart();
            if (accumulator_ > setpoint_)
                limited_value_ = saturate(unlimited_value, continouous_limit_);
            else
                limited_value_ = saturate(unlimited_value, min_limit_, max_limit_);
    }
    return limited_value_;
}

double Limiter::get_setpoint() const {
    return setpoint_;
}

double Limiter::get_accumulator() const {
    return accumulator_;
}

void Limiter::reset() {
    if (mode_ == Accumulate) {
        accumulator_ = 0.0;
        clock_.restart();
    }
}


};
