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
    bool update_channel(ChanNum channel_number) override;

    /// Enables or disables an encoder channel
    bool configure(ChanNum channel_number, bool enabled);

private:

    friend class MyRioConnector;

    void sync_from_myrio();
    void sync_to_myrio();
    bool on_enable() override;

    MyRioEncoder(MyRioConnector& connector, const ChanNums& channel_numbers);

private:

    MyRioConnector& connector_; ///< connector this module is on
};

} //namespace mel
