#pragma once
#include <vector>

namespace mel {

//-------------------------------------------------------------------------
// INDIVIDUAL FILTER IMPLEMENTATION
//-------------------------------------------------------------------------

class FilterImplementation {

public:

    FilterImplementation(const std::vector<double>& b, const std::vector<double>& a);

    void filter(const double& x, double& y);

    void reset(); /// sets the internal states s_ to all be zero

private:

    std::size_t n_; /// order

    const std::vector<double> b_; /// numerator coefficients
    const std::vector<double> a_; /// denominator coefficients

    std::vector<double> s_;
};

class Filter {

public:

    Filter();
    Filter(const std::vector<double>& b, const std::vector<double>& a);
    Filter(int length, const std::vector<double>& b, const std::vector<double>& a);

    void filter(const double& x, double& y);
    void filter(const std::vector<double>& x, std::vector<double>& y);

    void reset(); /// sets the internal states s_ to all be zero

private:

    const std::size_t length_;

    const std::vector<double> b_; /// numerator coefficients
    const std::vector<double> a_; /// denominator coefficients

    std::vector<FilterImplementation> filter_implementations_;
};

} // namespace mel

