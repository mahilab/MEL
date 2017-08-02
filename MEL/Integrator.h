#pragma once

class Integrator {

public:

    Integrator(double initial_value);

    double integrate(double x, double t);

    // initial conditions
    double initial_value_;

    // previous state
    double last_x_;
    double last_t_;
    
    // integrated value
    double integral_;
};