#pragma once

#include <MEL/Utility/Time.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a cyclic waveform with amplitude and offset in the time domain
class Waveform {

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
    Waveform(Type type, Time period, double amplitude = 1.0, double offset = 0.0);

    // Evaluates the Waform at Time t
    double evaluate(Time t);

public:

    Type type_;         ///< The waveform Type
    Time period_;       ///< The waveform period
    double amplitude_;  ///< The waveform amplitude
    double offset_;     ///< The waveform offset from zero

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
