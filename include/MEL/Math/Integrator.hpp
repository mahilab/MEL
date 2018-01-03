#pragma once

#include <MEL/Utility/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Integrates a continous waveform
class Integrator {

public:

    enum Technique {
        Trapezoidal,
        Simpsons
    };

    /// Default constructor
    Integrator(double initial_value, Technique technique = Trapezoidal);

    /// Integrats x with respect to time
    double integrate(double x, Time t);

    /// Resets the integrators
    void reset(double initial_value);

private:

    Technique technique_; ///< Integration technique to be used
    int step_count_;      ///< Running conter of calls to integrate()
    double last_last_x_;  ///< Integrand at two previous calls to integrate()
    double last_x_;       ///< Integrand at previous call to integrate()
    Time last_last_t_ ;   ///< Time at two previous calls to integrate()
    Time last_t_;         ///< Time at previous call to integrate()
    double integral_;     ///< The integral value

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
