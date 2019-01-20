// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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

#pragma once

#include <MEL/Config.hpp>
#include <MEL/Math/Process.hpp>
#include <MEL/Core/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Integrates a continous waveform
class MEL_API Integrator : public Process {
public:
    enum Technique { Trapezoidal, Simpsons };

    /// Constructor
    Integrator(double initial_value = 0.0, Technique technique = Trapezoidal);

    /// Set technique
    void set_technique(Technique technique);

    /// Integrats x with respect to time
    double update(double x, const Time& t) override;

    /// Resets the integrators
    void reset() override;

    /// Set the initial value
    void set_init(double initial_value);

private:
    Technique technique_;  ///< Integration technique to be used
    int step_count_;       ///< Running conter of calls to integrate()
    double last_last_x_;   ///< Integrand at two previous calls to integrate()
    double last_x_;        ///< Integrand at previous call to integrate()
    Time last_last_t_;     ///< Time at two previous calls to integrate()
    Time last_t_;          ///< Time at previous call to integrate()
    double integral_;      ///< The integral value
};

}  // namespace mel
