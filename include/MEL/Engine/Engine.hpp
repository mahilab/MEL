// MIT License
//
// MEL - Mechatronics Engine and Library
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
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_ENGINE_HPP
#define MEL_ENGINE_HPP

#include <MEL/Core/Timer.hpp>
#include <unordered_map>
#include <vector>

namespace mel {

class Object;

class Engine {
public:
    /// Constructor
    Engine(Object* root_object);

    /// Deconstructor
    ~Engine();

    /// Runs the Engine for the specified duration (indefintely by defalult)
    void run(Frequency loop_rate, Time duration = Time::Inf);

    /// Stops the Engine if it is running
    void stop();

private:

    Object* root_object_;
    Timer timer_;
    bool stop_;
};

}  // namespace mel

#endif  // MEL_ENGINE_HPP
