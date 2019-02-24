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

#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Daq/Encoder.hpp>
#include <utility>

namespace mel {

class MyRioConnector;

/// myRIO Encoder Module
class MyRioEncoder : public Encoder, NonCopyable {
public:

    /// Updates a single channel
    bool update_channel(ChanNum channel_number) override;

    /// Resets Encoder counts to a specifc value
    bool reset_count(ChanNum channel_number, int count) override;

    /// Enable a myRIO encoder channel
    void enable_channel(ChanNum channel_number);

    /// Disable a myRIO encoder channel
    void disable_channel(ChanNum channel_number_);

private:

    friend class MyRioConnector;

    void sync();

    MyRioEncoder(MyRioConnector& connector, const ChanNums& channel_numbers);

private:

    MyRioConnector& connector_;

    // NI FPGA Registers
    uint32_t              sysselect_;
    std::vector<int>      bits_;
    std::vector<uint32_t> stat_;
    std::vector<uint32_t> cntr_;
    std::vector<uint32_t> cnfg_;

    ChanNums              allowed_;  ///< allowed channels
};

} //namespace mel
