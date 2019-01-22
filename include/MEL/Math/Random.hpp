// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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

#pragma once

#include <MEL/Config.hpp>

namespace mel {

/// Returns an int random number in the interval [min, max].
int MEL_API random(int min, int max);

/// Returns an unsigned int random number in the interval [min, max].
unsigned int MEL_API random(unsigned int min, unsigned int max);

/// Returns a double random number in the interval [min, max].
double MEL_API random(double min, double max);

/// Returns a double random number in the interval [middle-deviation, middle+deviation].
double MEL_API randomDev(double middle, double deviation);

/// Sets the seed of the random number generator.
///
/// Setting the seed manually is useful when you want to reproduce a given sequence of random
/// numbers. Without calling this function, the seed is different at each program startup.
void MEL_API setRandomSeed(unsigned long seed);

}
