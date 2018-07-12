// MIT License
//
// MEL - Mechatronics Engine & Library
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
    Engine(Object* root_object = nullptr);

    /// Deconstructor
    ~Engine();

    /// Sets the root Object of the Engine
    void set_root_object(Object* root_object);

    /// Runs the Engine for the specified duration
    void run(Frequency loop_rate, Time duration = Time::Inf);

    /// Runs the Engine on a custom Timer for the specified duration
    void run(Timer timer, Time duration = Time::Inf);

    /// Runs the Engine without a Timer for a fixed number of iterations
    void run(uint32 iterations);

    /// Stops the Engine if it is running
    void stop();

    /// Resets the Engine
    void reset();

private:

    /// Boots the Eninge
    bool perfom_checks();

    Object* root_object_;
    Timer timer_;
    bool stop_;
};

}  // namespace mel

#endif  // MEL_ENGINE_HPP
