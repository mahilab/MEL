// MIT License
//
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_FILTER_HPP
#define MEL_FILTER_HPP

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
    Filter(const std::vector<double>& b, const std::vector<double>& a, int length = 1);
    //Filter(int length, const std::vector<double>& b, const std::vector<double>& a);

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

#endif // MEL_FILTER_HPP
