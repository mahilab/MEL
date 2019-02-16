// MIT License
//
// MEL - Mechatronics Engine and Library
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

#include <MEL/Daq/DaqBase.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>
#include <MEL/Daq/InputOutput.hpp>
#include <MEL/Daq/Encoder.hpp>
#include <MEL/Core/Clock.hpp>
#include <functional>

namespace mel {

class VirtualDaq;

//==============================================================================
// VIRTUAL AI
//==============================================================================

class MEL_API VirtualAI : public AnalogInput {
public:
    VirtualAI(VirtualDaq& daq, const std::vector<uint32>& channel_numbers);
    bool update_channel(uint32 channel_number) override;
public:
    Registry<std::function<Voltage(Time)>> sources;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL AO
//==============================================================================

class MEL_API VirtualAO : public AnalogOutput {
public:
    VirtualAO(VirtualDaq& daq, const std::vector<uint32>& channel_numbers);
    bool update_channel(uint32 channel_number) override;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL DI
//==============================================================================

class MEL_API VirtualDI : public DigitalInput {
public:
    VirtualDI(VirtualDaq& daq, const std::vector<uint32>& channel_numbers);
    bool update_channel(uint32 channel_number) override;
public:
    Registry<std::function<Logic(Time)>> sources;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL DO
//==============================================================================

class MEL_API VirtualDO : public DigitalOutput {
public:
    VirtualDO(VirtualDaq& daq, const std::vector<uint32>& channel_numbers);
    bool update_channel(uint32 channel_number) override;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL ENCODER
//==============================================================================

class MEL_API VirtualEncoder : public Encoder {
public:
    VirtualEncoder(VirtualDaq& daq, const std::vector<uint32>& channel_numbers);
    bool update_channel(uint32 channel_number) override;
public:
    Registry<std::function<int32(Time)>> sources;
private:
    VirtualDaq& daq_;
};

//==============================================================================
// VIRTUAL DAQ
//==============================================================================

/// Virtual DAQ for testing/prototyping the absence of actual hardware
class MEL_API VirtualDaq : public DaqBase {
public:
    VirtualDaq(const std::string& name);
    ~VirtualDaq();

    bool update_input() override;
    bool update_output() override;
public:
    VirtualAI AI;
    VirtualAO AO;
    VirtualDI DI;
    VirtualDO DO;
    VirtualEncoder encoder;

protected:
    bool on_enable() override;
    bool on_disable() override;
    bool on_open() override;
    bool on_close() override;

protected:
    friend class VirtualAI;
    friend class VirtualAO;
    friend class VirtualDI;
    friend class VirtualDO;
    friend class VirtualEncoder;
    Clock clock_;
};

}
