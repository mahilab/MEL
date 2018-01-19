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
    continouous_limit_(continuous_limit),
    min_limit_(-abs(abs_limit)),
    max_limit_(abs(abs_limit)),
    setpoint_( (sq(abs_limit) - sq(continuous_limit)) * time_limit.as_seconds() ),
    accumulator_(0.0),
    clock_(Clock())
{
    print(setpoint_);
}

double Limiter::limit(double unlimited_value) {
    switch(mode_) {
        case None:
            return unlimited_value;
        case Saturate:
            return saturate(unlimited_value, min_limit_, max_limit_);
        case Accumulate:
            accumulator_ += (sq(unlimited_value) - sq(continouous_limit_)) * clock_.get_elapsed_time().as_seconds();
            accumulator_ = saturate(accumulator_, 0.0, accumulator_);
            print(accumulator_);
            clock_.restart();
            if (accumulator_ > setpoint_)
                return saturate(unlimited_value, continouous_limit_);
            else
                return saturate(unlimited_value, min_limit_, max_limit_);
        default:
            return unlimited_value;
    }
}

void Limiter::reset() {
    if (mode_ == Accumulate) {
        accumulator_ = 0.0;
        clock_.restart();
    }
}


};
