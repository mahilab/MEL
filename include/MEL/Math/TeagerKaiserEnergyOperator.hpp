// MIT License
//
// MEL - MAHI Exoskeleton Library
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

#ifndef MEL_TEAGERKAISERENERGYOPERATOR_HPP
#define MEL_TEAGERKAISERENERGYOPERATOR_HPP

#include <MEL/Math/Process.hpp>
#include <vector>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class TeagerKaiserEnergyOperator : public Process {

public:

    /// Constructor
    TeagerKaiserEnergyOperator();

    /// applies the process operation for one sample
    double update(const double x, const Time& current_time = Time::Zero) override;

    /// resets internal memory
    void reset() override;

private:
    
    static const int n_ = 2; ///< size of internal memory, which is 2 for TKEO
   
    std::vector<double> s_; ///< internal memory
    
};


} // mel

#endif // MEL_TEAGERKAISERENERGYOPERATOR_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================