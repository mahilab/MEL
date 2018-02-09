#include <MEL/Math/Differentiator.hpp>

namespace mel {

Differentiator::Differentiator(Technique technique)
    : technique_(technique),
      step_count_(0),
      last_last_x_(0),
      last_x_(0),
      last_last_t_(Time::Zero),
      last_t_(Time::Zero),
      derivative_(0.0) {}

double Differentiator::differentiate(double x, Time t) {
    switch (technique_) {
        case BackwardDifference:
            if (step_count_ > 0) {
                derivative_ =
                    (x - last_x_) / (t.as_seconds() - last_t_.as_seconds());
            }
        case CentralDifference:
            if (step_count_ > 1) {
                derivative_ = (x - last_last_x_) /
                              (t.as_seconds() - last_last_t_.as_seconds());
            } else if (step_count_ > 0) {
                derivative_ =
                    (x - last_x_) / (t.as_seconds() - last_t_.as_seconds());
            }
    }
    last_last_x_ = last_x_;
    last_x_      = x;
    last_last_t_ = last_t_;
    last_t_      = t;
    ++step_count_;
    return derivative_;
}

}  // namespace mel
