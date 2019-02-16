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

#include <MEL/Daq/Encoder.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser implementation of Encoder
class MEL_API QuanserEncoder : public Encoder {
public:
    QuanserEncoder(QuanserDaq& daq, const std::vector<uint32>& channel_numbers);

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    bool reset_counts(const std::vector<int32>& counts) override;

    bool reset_count(uint32 channel_number, int32 count) override;

    bool set_quadrature_factors(
        const std::vector<QuadFactor>& factors) override;

    bool set_quadrature_factor(uint32 channel_number,
                               QuadFactor factor) override;

    const std::vector<uint32> get_quanser_velocity_channels();

private:
    QuanserDaq& daq_;  ///< Reference to parent QDaq
    Registry<uint32> velocity_channel_numbes_;  ///< Converted channel numbers

};

}  // namespace mel
