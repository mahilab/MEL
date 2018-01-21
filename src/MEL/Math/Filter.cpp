#include <MEL/Math/Filter.hpp>
#include <iostream>

namespace mel {

Filter::Filter() :
    Filter({ 0.020083365564211, 0.040166731128423, 0.020083365564211 },
           { 1.000000000000000, -1.561018075800718, 0.641351538057563 })
{ }

Filter::Filter(const std::vector<double>& b, const std::vector<double>& a) :
    Filter(1, b, a)
{ }

Filter::Filter(int length, const std::vector<double>& b, const std::vector<double>& a) :
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
        std::cout << "ERROR: Coefficient vector sizes do not match." << std::endl;
    }
    else if (a_.size() < 2) {
        std::cout << "ERROR: Coefficient vector must be longer than 1." << std::endl;
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
        std::cout << "ERROR: Input vector does not match size of filter bank." << std::endl;
        return;
    }
    filter_implementations_[0].filter(x, y);
}

void Filter::filter(const std::vector<double>& x, std::vector<double>& y) {

    if (x.size() != length_) {
        std::cout << "ERROR: Input vector does not match size of filter bank." << std::endl;
        return;
    }
    if (y.size() != length_) {
        std::cout << "ERROR: Output vector does not match size of filter bank." << std::endl;
        return;
    }

    for (int j = 0; j < length_; ++j) {
        filter_implementations_[j].filter(x[j], y[j]);
    }
}

void Filter::reset() {
    for (int j = 0; j < length_; ++j) {
        filter_implementations_[j].reset();
    }
}

void FilterImplementation::reset() {
    s_ = std::vector<double>(n_, 0.0);
}

} // namespace mel
