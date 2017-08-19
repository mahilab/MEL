#include "PdController.h"

namespace mel {

    double PdController::calculate(double x_ref, double x, double xdot_ref, double xdot) {
        e_ = x_ref - x;
        edot_ = xdot_ref - xdot;
        effort_ = kp_ * e_ + kd_ * edot_;
        return effort_;
    }

}