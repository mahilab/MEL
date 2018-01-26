// MIT License
//
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

#ifndef MEL_OWCONFIGURATION_HPP
#define MEL_OWCONFIGURATION_HPP

#include <MEL/Core/Amplifier.hpp>
#include <MEL/Daq/Daq.hpp>
#include <MEL/Daq/Encoder.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>
#include <MEL/Daq/Velocity.hpp>
#include <MEL/Daq/Watchdog.hpp>
#include <vector>


namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class OpenWrist;
class Q8Usb;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates the hardware configuration for an OpenWrist
struct OwConfiguration {
public:
    /// Generic Configuration 1 (creates Amplifiers)
    OwConfiguration(Daq& daq,
                    Watchdog& watchdog,
                    const std::vector<Amplifier::TtlLevel>& enable_levels,
                    const std::vector<DigitalOutput::Channel>& enable_channels,
                    const std::vector<double>& command_gains,
                    const std::vector<AnalogOutput::Channel>& command_channels,
                    const std::vector<Limiter>& amp_current_limiters,
                    const std::vector<Amplifier::TtlLevel>& fault_levels,
                    const std::vector<DigitalInput::Channel>& fault_channels,
                    const std::vector<double>& sense_gains,
                    const std::vector<AnalogInput::Channel>& sense_channel,
                    const std::vector<Encoder::Channel>& encoder_channels,
                    const std::vector<Velocity::Channel>& velocity_channels);

    /// Generic Configuration 2 (given Amplifiers)
    OwConfiguration(Daq& daq,
                    Watchdog& watchdog,
                    const std::vector<Encoder::Channel>& encoder_channels,
                    const std::vector<Velocity::Channel>& velocity_channels,
                    const std::vector<Amplifier>& amplifiers);


private:

    friend class OpenWrist;

    Daq& daq_;            ///< DAQ controlling the OpenWrist
    Watchdog& watchdog_;  ///< watchdog the OpenWrist is guarded by
    std::vector<Encoder::Channel>
        encoder_channels_;  ///< encoder channels that measure motor positions
    std::vector<Velocity::Channel>
        velocity_channels_;  ///< encoder channels that measure motor velocities
    std::vector<Amplifier> amplifiers_;  ///< the amplifiers being use to
                                         ///< control the OpenWrist's motors
};

}  // namespace mel

#endif  // MEL_OWCONFIGURATION_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
