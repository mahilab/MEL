#include "Integrator.h" 

namespace mel {

    Integrator::Integrator(double initial_condition, Integrator::Technique technique) : integral_(initial_condition), technique_(technique) {}

    double Integrator::integrate(double x, double t) {
        switch (technique_) {
        case Technique::Trapezoidal:
            if (step_count_ > 0)
                integral_ += (t - last_t_) * (0.5 * (last_x_ + x));
            break;
        case Technique::Simpsons:
            integral_;
            if (step_count_ > 1)
                integral_ += 0.166666666666666 * (t - last_t_) * (last_last_x_ + 4 * last_x_ + x);
            else if (step_count_ > 0)
                integral_ += (t - last_t_) * (0.5 * (last_x_ + x));
            break;
        }
        last_last_x_ = last_x_;
        last_last_t_ = last_t_;
        last_x_ = x;
        last_t_ = t;
        step_count_ += 1;
        return integral_;
    }

    void Integrator::reset(double initial_value) {
        integral_ = initial_value;
        step_count_ = 0;
        last_last_x_ = 0.0;
        last_last_t_ = 0.0;
        last_x_ = 0.0;
        last_t_ = 0.0;
    }
}