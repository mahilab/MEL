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

#include <MEL/Core/Timer.hpp>
#include <MEL/Math/Integrator.hpp>
#include <MEL/Math/Differentiator.hpp>
#include <MEL/Math/Butterworth.hpp>

namespace mel {

class PidController {
public:
    /// Constructor
    PidController(double kp = 0.0, double ki = 0.0, double kd = 0.0);

    /// Calculates the control effort given the desired reference and actual current state
    double operator()(double x_ref, double x, Time t);

    /// Calculates the control effort given the desired reference and actual current state
    double calculate(double x_ref, double x, Time t);

    /// Calculates the control effort given the desired reference and actual current state and state derivative
    double operator()(double x_ref, double x, double xdot, Time t);

    /// Calculates the control effort given the desired reference and actual current state and state derivative
    double calculate(double x_ref, double x, double xdot, Time t);

    /// Rests the PID Inegrator and Differentiator
    void reset();

public:

    double kp;  ///< the proportional control gain
    double ki;  ///< the integral control gain
    double kd;  ///< the derivative control gain
    Integrator integrator; ///< PID integrator
    Differentiator differentiator; ///< PID differentiator
    Butterworth filter;  ///< PID velocity filter
};

} // namespace mel
