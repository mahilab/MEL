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

namespace mel {

class MyRioConnector;

/// myRIO Encoder Module
class MEL_API MyRioEncoder : public Encoder, NonCopyable {
public:

    /// Updates a single channel
    bool update_channel(uint32 channel_number) override;

private:

    friend class MyRioConnector;

    MyRioEncoder(MyRioConnector& connector, const std::vector<uint32>& channel_numbers);
    bool on_enable() override;

private:

    const MyRioConnector& connector_; ///< connector this module is on
};

} //namespace mel
