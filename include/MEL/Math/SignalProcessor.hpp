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
#include <MEL/Utility/Console.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class SignalProcessor {

public:

    /// Constructor
    SignalProcessor(std::vector<Process*> processes) :
        processes_(processes),
        n_(processes.size()),
        s_(n_),
        x_(0.0)
    { }

    /// Default destructor
    ~SignalProcessor() {};

    /// applies the process operations for one sample
    double update(const double x, const Time& current_time = Time::Zero) {
        if (n_ > 0) {
            x_ = x;
            s_[0] = processes_[0]->update(x_, current_time);
            for (size_t i = 1; i < n_; ++i) {
                s_[i] = processes_[i]->update(s_[i-1], current_time);
            }
        }
        else {
            return x;
        }
    }

    /// get internal states
    double get_unprocessed() const {
        return x_;
    }

    /// get internal states
    const std::vector<double>& get_processed() const {
        return s_;
    }

    /// resets internal memory
    void reset() {
        x_ = 0.0;
        if (n_ > 0) {
            s_ = std::vector<double>(n_);
            for (size_t i = 0; i < n_; ++i) {
                processes_[i]->reset();
            }
        }
    };

    /// returns a process pointer
    template <class T = Process>
    T& get_process(const std::size_t index) const {
        if (index >= 0 & index < n_) {
            return *dynamic_cast<T*>(processes_[index]);
        }
        else {
            return *dynamic_cast<T*>(processes_[0]);
        }
    }

private:

    
    std::vector<Process*> processes_; ///< chain of signal processing operations to be carried out sequentially

    size_t n_; ///< size of internal memory
    
    std::vector<double> s_; ///< internal memory

    double x_; ///< internal memory

};


} // mel

#endif // MEL_SIGNALPROCESSOR_HPP
