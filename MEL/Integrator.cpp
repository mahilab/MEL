#include "Integrator.h" 

Integrator::Integrator(double initial_value) : initial_value_(initial_value), integral_(initial_value) {}

double Integrator::integrate(double x, double t) {
    integral_ += (t - last_t_) * ((last_x_ + x) / 2.0);
    last_x_ = x;
    last_t_ = t;
    return integral_;
}

