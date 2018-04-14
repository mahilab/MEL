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
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_QOPTIONS_HPP
#define MEL_QOPTIONS_HPP

#include <MEL/Utility/Types.hpp>
#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Utility class for creating Quanser configuration strings
class QOptions {
public:
    enum class UpdateRate {
        Normal = 1,  ///< 1 kHz
        Fast   = 2   ///< 8 kHz
    };

    enum class EncoderDirection { Nonreversed = 0, Reversed = 1 };

    enum class EncoderFilter { Unfiltered = 0, Filtered = 1 };

    enum class EncoderDetection { High = 0, Low = 1 };

    enum class EncoderReload { NoReload = 0, OnPulse = 1 };

    enum class AoMode {
        VoltageMode  = 0,
        CurrentMode2 = 1,
        CurrentMode1 = 2,
        CurrentMode0 = 3,
        ControlMode2 = 4,
        ControlMode1 = 5,
        ControlMode0 = 6
    };

    enum class LedMode { Auto = 0, User = 1 };

public:
    QOptions();

    void set_update_rate(UpdateRate update_rate);

    void set_decimation(uint32 decimation);

    void set_encoder_direction(uint32 channel_number,
                               EncoderDirection direction);

    void set_encoder_filter(uint32 channel_number, EncoderFilter filter);

    void set_encoder_detection_a(uint32 channel_number,
                                 EncoderDetection detection);

    void set_encoder_detection_b(uint32 channel_number,
                                 EncoderDetection detection);

    void set_encoder_detection_z(uint32 channel_number,
                                 EncoderDetection detection);

    void set_encoder_reload(uint32 channel_number, EncoderReload reload);

    void set_encoder_velocity(uint32 channel_number, double velocity);

    void set_analog_output_mode(uint32 channel_number,
                                AoMode mode,
                                double kff,
                                double a0,
                                double a1,
                                double a2,
                                double b0,
                                double b1,
                                double post);

    void set_special_option(std::string option);

    void set_led_mode(LedMode mode);

    std::string get_string();

private:
    UpdateRate update_rate_;  ///< update rate
    uint32 decimation_;       ///< decimation rate
    std::string options_;     ///< the configuration options string
};

}  // namespace mel

#endif  // MEL_QOPTIONS_HPP

// http://www.quanser.com/Products/quarc/documentation/q8_usb.html
