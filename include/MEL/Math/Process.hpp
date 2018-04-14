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

#ifndef MEL_PROCESS_HPP
#define MEL_PROCESS_HPP

#include <MEL/Core/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class Process {
public:
    /// Default constructor
    Process(){};

    /// Default destructor
    virtual ~Process(){};

    /// applies the process operation for one sample
    virtual double update(const double x,
                          const Time& current_time = Time::Zero) {
        Time unused = current_time;  // unused
        return x;
    }

    /// resets internal memory
    virtual void reset(){};

private:
};

}  // namespace mel

#endif  // MEL_PROCESS_HPP
