#pragma once

namespace mel {

class PdController {

public:

    /// Default constructor
    PdController() : kp_(0.0), kd_(0.0) {}
    /// Prefered constructor
    PdController(double kp, double kd) : kp_(kp), kd_(kd) {};

    /// Calculates the control effort given the current state and desired reference
    double calculate(double x_ref, double x, double xdot_ref, double xdot);

    /// Computes the control effort to move to a desired location x_ref with a constant velocity xdot_ref, and the hold that position
    /// start_here should be true the first time this function is called in a loop, and subsequently false once movement has initiated
    double move_to_hold(double x_ref, double x, double xdot_ref, double xdot, double delta_time, double window, bool start_here);

    /// Computes the control effort to move to a desired location #x_ref with a constant velocity #xdot_ref, then hold that position.
    /// #delta_time should be the elapsed time since the last call to this function (e.g. Clock.delta_time_). #hold_tol is the tolerance
    /// within which the controller switches from the moving state to the holding state. #break_tol is the tolerance within which the
    /// controller switches from a holding state to a moving state; it should be larger than #hold_tol
    double move_to_hold(double x_ref, double x, double xdot_ref, double xdot, double delta_time, double hold_tol, double break_tol);

    /// Resets move_to_hold function. Call this before calling move_to_hold again if there was a period of inactivity since the last call
    /// to move_to_hold. If this isn't called and move_to_hold is within the break_tol, the controller will snap to x_ref instead of
    /// moving smoothly to it.
    void reset_move_to_hold();

    double kp_; ///< the proportional control gain
    double kd_; ///< the derivative control gain

    double e_; ///< the error between the process varable x and the setpoint x_ref
    double edot_; ///< the error between xdot and xdot_ref
    double effort_; ///< the calculated control effort from the PD controller

private:

    //---------------------------------------------------------------------
    // PRIVATE VARIABLES
    //---------------------------------------------------------------------

    double last_x_;

    bool holding_      = false;
    bool move_started_ = false;

};

}
