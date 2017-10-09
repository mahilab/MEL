#pragma once

namespace mel {

    namespace core {

        class PdController {

        public:

            /// Default constructor
            PdController() : kp_(0.0), kd_(0.0) {}
            /// Prefered constructor
            PdController(double kp, double kd) : kp_(kp), kd_(kd) {};

            /// Calculates the control effort given the current state and desired reference 
            double calculate(double x_ref, double x, double xdot_ref, double xdot);

            /// Computes the control effort to move to a desired location x_ref with a constant velocity x_ref, and the hold that position
            /// state_here should be true the first time this function is called in a loop, and subsequently false once movement has initiated
            double move_to_hold(double x_ref, double x, double xdot_ref, double xdot, double delta_time, double window, bool start_here);
            double move_to_hold(double x_ref, double x, double xdot_ref, double xdot, double delta_time, double min_tol, double max_tol);

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

            bool move_started_ = false;

        };

    }

}