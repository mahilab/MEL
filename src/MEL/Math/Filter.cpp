#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Filter.hpp>
#include <iostream>

namespace mel {

Filter::Filter(const std::vector<double>& b, const std::vector<double>& a, uint32 seeding) : Process(),
    has_seeding_(seeding > 0),
    first_update_(true),
    seed_count_(seeding)
{
    set_coefficients(b, a);
}

Filter::Filter(std::size_t n, uint32 seeding) : Process(),
    n_(n),
    s_(n_, 0.0),
    has_seeding_(seeding > 0),
    first_update_(true),
    seed_count_(seeding)
{}

double Filter::update(const double x, const Time& current_time) {
    Time unused = current_time; // unused
    if (first_update_) {
        if (has_seeding_) {
            seed(x, seed_count_);
        }
        first_update_ = false;
    }  
    return dir_form_ii_t(x);
}

void Filter::reset() {
    s_ = std::vector<double>(n_, 0.0);
    first_update_ = true;
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

double Filter::dir_form_ii_t(const double x) {
    double y = (s_[0] + b_[0] * x) / a_[0];
    for (std::size_t i = 0; i < n_ - 1; ++i) {
        s_[i] = s_[i + 1] + b_[i + 1] * x - a_[i + 1] * y;
    }
    s_[n_ - 1] = b_[n_] * x - a_[n_] * y;
    return y;
}

void Filter::seed(const double x, const uint32 iterations) {
    for (uint32 i = 0; i < iterations; ++i) {
        dir_form_ii_t(x);
    }
}



}  // namespace mel
