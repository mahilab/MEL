#include <MEL/Math/Filter.hpp>
#include <MEL/Logging/Log.hpp>
#include <iostream>

namespace mel {

Filter::Filter(const std::vector<double>& b, const std::vector<double>& a) :
    Process(),
    b_(b),
    a_(a)
{
    if (a_.size() != b_.size()) {
        LOG(Error) << "Filter coefficient vector sizes do not match";
    }
    else if (a_.size() < 2) {
        LOG(Error) << "Coefficient vector must be longer than 1";
    }
    else {
        n_ = a_.size() - 1;
        s_ = std::vector<double>(n_, 0.0);
    }
}

double Filter::process(const double x, const Time& current_time) {
    double y;
    y = (s_[0] + b_[0] * x) / a_[0];
    for (std::size_t i = 0; i < n_ - 1; ++i) {
        s_[i] = s_[i + 1] + b_[i + 1] * x - a_[i + 1] * y;
    }
    s_[n_ - 1] = b_[n_] * x - a_[n_] * y;
    return y;
}





void Filter::reset() {
        s_ = std::vector<double>(n_, 0.0);
    }

} // mel
