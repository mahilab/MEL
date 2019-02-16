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

#include <MEL/Daq/Input.hpp>
#include <MEL/Core/NonCopyable.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class MEL_API QuanserDI : public DigitalInput, NonCopyable {
public:
    QuanserDI(QuanserDaq& daq, const std::vector<uint32>& channel_numbers);

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    std::vector<char>& get_quanser_values();

private:
    QuanserDaq& daq_;  ///< Reference to parent QDaq
    Registry<char> quanser_values_;
};

}  // namespace mel
