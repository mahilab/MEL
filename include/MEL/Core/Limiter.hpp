#pragma once

#include <MEL/Utility/Clock.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Limiter {

public:

    /// Default constructor. No limit imposed.
    Limiter();

    /// Limiter that saturates value based on a absolute value
    Limiter(double abs_limit);

    /// Limiter that saturates values based on separate min and max values
    Limiter(double min_limit, double max_limit);

    /// Limiter that uses an accumulation algorithm (aka i^2*t)
    Limiter(double continuous_limit, double peak_limit, Time time_limit);

    /// Limits the unlimited value using the Limiter mode
    double limit(double unlimited_value);

    /// Resets the Limiter accumulator and clock
    void reset();

private:

    enum Mode {
        None,
        Saturate,
        Accumulate
    };

    Mode mode_;
    double min_limit_;
    double max_limit_;
    double continouous_limit_;
    double setpoint_;
    double accumulator_;
    Clock clock_;

};

} // namespace mel
