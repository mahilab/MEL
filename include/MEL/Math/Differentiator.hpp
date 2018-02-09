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

#ifndef MEL_DIFFERENTIATOR_HPP
#define MEL_DIFFERENTIATOR_HPP

#include <MEL/Math/Process.hpp>
#include <MEL/Utility/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATIONS
//==============================================================================

class Differentiator : public Process {
public:
    enum Technique { BackwardDifference, CentralDifference };

public:

    /// Constructor
    Differentiator(Technique technique = BackwardDifference);

    /// Differentiates a signal
    double update(const double x, const Time& t);

    /// Resets the differentiator
    void reset();

private:
    Technique technique_;  ///< Differentiation technique to be used
    int step_count_;       ///< Running conter of calls to integrate()
    double last_last_x_;   ///< Integrand at two previous calls to integrate()
    double last_x_;        ///< Integrand at previous call to integrate()
    Time last_last_t_;     ///< Time at two previous calls to integrate()
    Time last_t_;          ///< Time at previous call to integrate()
    double derivative_;    ///< The derivative value
};

}  // namespace mel

#endif  // MEL_DIFFERENTIATOR_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
