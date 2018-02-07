#include <MEL/Math/Filter.hpp>
#include <MEL/Logging/Log.hpp>
#include <iostream>

namespace mel {

Filter::Filter() :
    Filter({ 0.020083365564211, 0.040166731128423, 0.020083365564211 },
           { 1.000000000000000, -1.561018075800718, 0.641351538057563 })
{ }

//Filter::Filter(const std::vector<double>& b, const std::vector<double>& a) :
//    Filter(1, b, a)
//{ }

Filter::Filter(const std::vector<double>& b, const std::vector<double>& a, int length) :
    length_(length),
    b_(b),
    a_(a),
    filter_implementations_(std::vector<FilterImplementation>(length, FilterImplementation(b, a)))
{ }

FilterImplementation::FilterImplementation(const std::vector<double>& b, const std::vector<double>& a) :
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


void FilterImplementation::filter(const double& x, double& y) {
    y = (s_[0] + b_[0] * x) / a_[0];
    for (std::size_t i = 0; i < n_ - 1; ++i) {
        s_[i] = s_[i + 1] + b_[i + 1] * x - a_[i + 1] * y;
    }
    s_[n_ - 1] = b_[n_] * x - a_[n_] * y;
}

void Filter::filter(const double& x, double& y) {
    if (length_ != 1) {
        LOG(Error) << "ERROR: Input vector does not match size of filter bank.";
        return;
    }
    filter_implementations_[0].filter(x, y);
}

void Filter::filter(const std::vector<double>& x, std::vector<double>& y) {

    if (x.size() != length_) {
        LOG(Error) << "ERROR: Input vector does not match size of filter bank.";
        return;
    }
    if (y.size() != length_) {
        LOG(Error) << "ERROR: Output vector does not match size of filter bank.";
        return;
    }

    for (std::size_t j = 0; j < length_; ++j) {
        filter_implementations_[j].filter(x[j], y[j]);
    }
}

void Filter::reset() {
    for (std::size_t j = 0; j < length_; ++j) {
        filter_implementations_[j].reset();
    }
}

void FilterImplementation::reset() {
    s_ = std::vector<double>(n_, 0.0);
}

} // namespace mel
