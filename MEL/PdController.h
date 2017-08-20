#pragma once
#include "Clock.h"

namespace mel {


    class PdController {

    public:
 
        /// Default constructor
        PdController() : kp_(0.0), kd_(0.0) {}
        /// Prefered constructor
        PdController(double kp, double kd) : kp_(kp), kd_(kd) {};

        /// Calculates the control effort given the current state and desired reference 
        double calculate(double x_ref, double x, double xdot_ref, double xdot);

        double move_to_hold(double x_ref, double x, double xdot_ref, double xdot, double delta_time, double window, bool start_here);

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
        bool holding_;

    };

}