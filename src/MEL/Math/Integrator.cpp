#include <MEL/Math/Integrator.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Integrator::Integrator(double initial_condition,
                       Integrator::Technique technique)
    : technique_(technique),
      step_count_(0),
      last_last_x_(0),
      last_x_(0),
      last_last_t_(Time::Zero),
      last_t_(Time::Zero),
      integral_(initial_condition) {}

double Integrator::integrate(double x, Time t) {
    switch (technique_) {
        case Technique::Trapezoidal:
            if (step_count_ > 0)
                integral_ += (t.as_seconds() - last_t_.as_seconds()) *
                             (0.5 * (last_x_ + x));
            break;
        case Technique::Simpsons:
            if (step_count_ > 1)
                integral_ += 0.166666666666666 *
                             (t.as_seconds() - last_t_.as_seconds()) *
                             (last_last_x_ + 4 * last_x_ + x);
            else if (step_count_ > 0)
                integral_ += (t.as_seconds() - last_t_.as_seconds()) *
                             (0.5 * (last_x_ + x));
            break;
    }
    last_last_x_ = last_x_;
    last_x_      = x;
    last_last_t_ = last_t_;
    last_t_      = t;
    step_count_ += 1;
    return integral_;
}

void Integrator::reset(double initial_value) {
    integral_    = initial_value;
    step_count_  = 0;
    last_last_x_ = 0.0;
    last_x_      = 0.0;
    last_last_t_ = Time::Zero;
    last_t_      = Time::Zero;
}

}  // namespace mel
