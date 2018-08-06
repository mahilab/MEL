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

#ifndef MEL_VIRTUAL_DAQ_HPP
#define MEL_VIRTUAL_DAQ_HPP

#include <MEL/Daq/DaqBase.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>
#include <MEL/Daq/InputOutput.hpp>
#include <MEL/Daq/Encoder.hpp>
#include <MEL/Core/Clock.hpp>

namespace mel {

class VirtualDaq;

class VirtualAI : public AnalogInput {
public:
    bool update_channel(uint32 channel_number) override;
};

class VirtualAO : public AnalogOutput {
public:
    bool update_channel(uint32 channel_number) override;
};

class VirtualDI : public DigitalInput {
public:
    bool update_channel(uint32 channel_number) override;
};

class VirtualDO : public DigitalOutput {
public:
    bool update_channel(uint32 channel_number) override;
};

class VirtualDIO : public DigitalInputOutput {
public:
    bool update_channel(uint32 channel_number) override;
};

class VirtualEncoder : public Encoder {
public:
    bool update_channel(uint32 channel_number) override;
};

/// Virtual DAQ for testing/prototyping the absence of actual hardware
class VirtualDaq : public DaqBase {
public:
    VirtualDaq(const std::string& name);
    bool enable() override;
    bool disable() override;
    bool open() override;
    bool close() override;
    bool update_input() override;
    bool update_output() override;
public:
    VirtualAI AI;
    VirtualAO AO;
    VirtualDI DI;
    VirtualDO DO;
    VirtualDIO DIO;
    VirtualEncoder encoder;
private:
    Clock clock_;
};

}

#endif // MEL_VIRTUAL_DAQ_HPP
