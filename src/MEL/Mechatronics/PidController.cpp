#include <MEL/Mechatronics/PidController.hpp>
#include <MEL/Math/Functions.hpp>

namespace mel {

PidController::PidController(double _kp, double _ki, double _kd) :
    kp(_kp),
    ki(_ki),
    kd(_kd)
{ 
    reset();
}

double PidController::operator()(double x_ref, double x, Time t) {
    return calculate(x_ref, x, t);
}

double PidController::calculate(double x_ref, double x, Time t) {
    double e = x_ref - x;
    double ei = integrator.update(e, t);
    double ed = differentiator.update(e, t);
           ed = filter.update(ed, t);
    return kp*e + ki*ei + kd*ed;
}

double PidController::operator()(double x_ref, double x, double xdot, Time t) {
    return calculate(x_ref, x, xdot, t);
}

double PidController::calculate(double x_ref, double x, double xdot, Time t) {
    double e = x_ref - x;
    double ei = integrator.update(e, t);
    double ed = 0 - xdot;
           ed = filter.update(ed, t);
    return kp * e + ki * ei + kd * ed;
}

void PidController::reset() {
    integrator.reset();
    differentiator.reset();
}


} // namespace mel
