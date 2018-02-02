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

#ifndef MEL_INTEGRATOR_HPP
#define MEL_INTEGRATOR_HPP

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

#endif // MEL_INTEGRATOR_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
