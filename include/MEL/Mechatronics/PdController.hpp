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


namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class PdController {
public:
    /// Constructor
    PdController(double kp = 0.0, double kd = 0.0);

    /// Calculates the control effort given the current state and desired reference
    double operator()(double x_ref, double x, double xdot_ref, double xdot);

    /// Calculates the control effort given the current state and desired reference
    double calculate(double x_ref, double x, double xdot_ref, double xdot);

    /// Computes the control effort to move to a desired location #x_ref with a
    /// constant velocity #xdot_ref, then hold that position.
    /// #delta_time should be the elapsed time since the last call to this
    /// function (e.g. Clock.delta_time_). #hold_tol is the tolerance
    /// within which the controller switches from the moving state to the
    /// holding state. #break_tol is the tolerance within which the
    /// controller switches from a holding state to a moving state; it should
    /// be larger than #hold_tol
    double move_to_hold(double x_ref,
                        double x,
                        double xdot_ref,
                        double xdot,
                        double delta_time,
                        double hold_tol,
                        double break_tol);

    /// Resets move_to_hold function. Call this before calling move_to_hold
    /// again if there was a period of inactivity since the last call
    /// to move_to_hold. If this isn't called and move_to_hold is within the
    /// break_tol, the controller will snap to x_ref instead of
    /// moving smoothly to it.
    void reset_move_to_hold();

public:

    double kp;  ///< the proportional control gain
    double kd;  ///< the derivative control gain

private:

    double last_x_;      ///< the last x used for move to hold
    bool holding_;       ///< true if holding
    bool move_started_;  ///< true if moving
};

}  // namespace mel
