// MIT License
//
// MEL - Mechatronics Library
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

#include <MEL/Math/Process.hpp>
#include <vector>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Filter : public Process {
public:
    /// Construct Filter from transfer function coefficients
    Filter(const std::vector<double>& b,
           const std::vector<double>& a,
           uint32 seeding = 0);

    /// Applies the filter operation for one time step
    double update(const double x,
                  const Time& current_time = Time::Zero) override;

    /// Sets the internal states s_ to all be zero
    void reset() override;

    /// Returns the Filter numerator coefficients
    const std::vector<double>& get_b() const;

    /// Returns the Filter denominator coefficients
    const std::vector<double>& get_a() const;

    /// Sets the Filter coefficients
    void set_coefficients(const std::vector<double>& b,
                          const std::vector<double>& a);

protected:
    /// Construct empty Filter of order n
    Filter(std::size_t n, uint32 seeding);

private:
    /// Direct form II transposed filter implementation
    double dir_form_ii_t(const double x);

    /// Calls the Filter multiple times on the initial value to avoid startup
    /// transients
    void seed(const double x, const uint32 iterations);

protected:
    std::size_t n_;          ///< filter order
    std::vector<double> b_;  ///< numerator coefficients
    std::vector<double> a_;  ///< denominator coefficients
    std::vector<double> s_;  ///< internal memory
    bool has_seeding_;       ///< indicates whether or not to call seed on first
                             ///< update
    bool first_update_;      ///< indicates first update upon reset
    uint32
        seed_count_;  ///< number of iterations to call on update upon seeding
};

}  // namespace mel

#endif  // MEL_FILTER_HPP
