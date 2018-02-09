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

#ifndef MEL_SIGNALPROCESSOR_HPP
#define MEL_SIGNALPROCESSOR_HPP

#include <MEL/Math/Process.hpp>
#include <vector>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class SignalProcessor {

public:

    /// Constructor
    SignalProcessor(std::vector<Process*> processes) :
        processes_(processes),
        n_(processes.size() + 1),
        s_(n_)
        { };

    /// Default destructor
    ~SignalProcessor() {};

    /// applies the process operations for one sample
    double process(const double x, const Time& current_time = Time::Zero) {
        if (n_ > 0) {
            s_[0] = x;
            for (size_t i = 1; i < n_; ++i) {
                s_[i] = processes_[i-1]->process(s_[i-1], current_time);
            }
        }
        else {
            return 0.0;
        }
    }

    /// get internal states
    double get_signal(const std::size_t index) const {
        if (index > 0 & index < n_) {
            return s_[index];
        }
        else {
            return 0.0;
        }
    }

    /// resets internal memory
    void reset() {
        if (n_ > 0) {
            s_ = std::vector<double>(n_);
            for (size_t i = 1; i < n_; ++i) {
                processes_[i-1]->reset();
            }
        }
    };

private:

    
    std::vector<Process*> processes_; ///< chain of signal processing operations to be carried out sequentially
   
    size_t n_; ///< size of internal memory
    
    std::vector<double> s_; ///< internal memory

};


} // mel

#endif // MEL_SIGNALPROCESSOR_HPP
