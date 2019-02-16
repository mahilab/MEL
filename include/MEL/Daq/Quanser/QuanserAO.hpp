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

#include <MEL/Daq/Output.hpp>
#include <MEL/Core/NonCopyable.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class MEL_API QuanserAO : public AnalogOutput, NonCopyable {
public:
    QuanserAO(QuanserDaq& daq, const std::vector<uint32>& channel_numbers);

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    bool set_ranges(const std::vector<Voltage>& min_values,
                    const std::vector<Voltage>& max_values) override;

    bool set_range(uint32 channel_number,
                   Voltage min_value,
                   Voltage max_value) override;

    bool set_expire_values(const std::vector<Voltage>& expire_values) override;

    bool set_expire_value(uint32 channel_number, Voltage expire_value) override;

private:
    QuanserDaq& daq_;  ///< Reference to parent QDaq
};

}  // namespace mel
