#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Filter.hpp>
#include <iostream>

namespace mel {

Filter::Filter(const std::vector<double>& b, const std::vector<double>& a)
    : Process()
{
    set_coefficients(b, a);
}

Filter::Filter(std::size_t n) : Process(), n_(n), s_(n_, 0.0) {}

double Filter::update(const double x, const Time& current_time) {
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

const std::vector<double>& Filter::get_b() const {
    return b_;
}

const std::vector<double>& Filter::get_a() const {
    return a_;
}

void Filter::set_coefficients(const std::vector<double>& b,
                              const std::vector<double>& a) {
    if (a.size() != b.size()) {
        LOG(Error) << "Filter coefficient vector sizes do not match";
    } else if (a.size() < 2) {
        LOG(Error) << "Coefficient vectors must be longer than length 1";
    } else {
        b_ = b;
        a_ = a;
        n_ = a_.size() - 1;
        s_ = std::vector<double>(n_, 0.0);
    }
}

}  // namespace mel
