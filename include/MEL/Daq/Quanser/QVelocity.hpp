// MIT License
//
// MEL - Mechatronics Engine & Library
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

#ifndef MEL_QVELOCITY_HPP
#define MEL_QVELOCITY_HPP

#include <MEL/Daq/Velocity.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser implementation of Encoder
class QVelocity : public Velocity {
public:
    QVelocity(QDaq& daq, const std::vector<uint32>& channel_numbers);

    ~QVelocity();

    bool enable() override;

    bool disable() override;

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    bool set_quadrature_factors(
        const std::vector<QuadFactor>& factors) override;

    bool set_quadrature_factor(uint32 channel_number,
                               QuadFactor factor) override;

    const std::vector<uint32>& get_converted_channel_numbers();

private:
    /// Quanser velocity channels start on 14000. This ensures that 0 based and
    /// 14000 based channel numbers are always converted to 14000 based.
    static uint32 convert_channel_number(uint32 channel_number);

    /// Performs convert_channel_number on a vector of channel numbers
    static std::vector<uint32> convert_channel_numbers(
        const std::vector<uint32>& channel_numbers);

private:
    const std::vector<uint32>
        converted_channel_numbers_;  ///< Converted channel numbers
    QDaq& daq_;                      ///< Reference to parent QDaq
};

}  // namespace mel

#endif  // MEL_QVELOCITY_HPP
