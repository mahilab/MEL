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

#ifndef MEL_WAVEFORM_HPP
#define MEL_WAVEFORM_HPP

#include <MEL/Config.hpp>
#include <MEL/Core/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a cyclic waveform with amplitude and offset in the time domain
class MEL_API Waveform {
public:
    /// The Type of Waveform
    enum Type {
        Sin,       ///< Sine waveform
        Cos,       ///< Cosine waveform
        Square,    ///< Square waveform
        Triangle,  ///< Triangle waveform
        Sawtooth   ///< Sawtooth waveform
    };

public:
    // Default constructor
    Waveform(Type type,
             Time period,
             double amplitude = 1.0,
             double offset    = 0.0);

    // Evaluates the Waform at Time t
    double evaluate(Time t);

public:
    Type type_;         ///< The waveform Type
    Time period_;       ///< The waveform period
    double amplitude_;  ///< The waveform amplitude
    double offset_;     ///< The waveform offset from zero
};

}  // namespace mel

#endif  // MEL_WAVEFORM_HPP
