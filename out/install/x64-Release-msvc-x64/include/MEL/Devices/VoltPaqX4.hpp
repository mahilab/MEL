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
#include <MEL/Mechatronics/Amplifier.hpp>
#include <MEL/Core/Device.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a Quanser VoltPAQ-X4 Linear Voltage Amplifier
class VoltPaqX4 : public Device {
public:
    /// Constructor
    VoltPaqX4(const std::vector<DigitalOutput::Channel>& enable_channels,
              const std::vector<AnalogOutput::Channel>& command_channels,
              const std::vector<DigitalInput::Channel>& fault_channels,
              const std::vector<AnalogInput::Channel>& sense_channels);

    std::vector<Amplifier> amplifiers;  ///< the individual amplifiers

private:
    /// Enables all VoltPAQ-X4 amplifiers
    bool on_enable() override;

    /// Disables all VoltPAQ-X4 amplifiers
    bool on_disable() override;

};

}  // namespace mel
