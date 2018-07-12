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

#ifndef MEL_MYRIOANALOGINPUT_HPP
#define MEL_MYRIOANALOGINPUT_HPP

#include <MEL/Daq/Input.hpp>
#include <MEL/Utility/NonCopyable.hpp>

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

namespace mel {

class MyRio;


//==============================================================================
// CLASS DECLARATION
//==============================================================================

class MyRioAnalogInput : public AnalogInput, NonCopyable {
public:

    MyRioAnalogInput(MyRio& daq);

    ~MyRioAnalogInput();

    bool enable() override;

    bool disable() override;

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    // bool set_ranges(const std::vector<Voltage>& min_values,
    //                 const std::vector<Voltage>& max_values) override {}

    // bool set_range(uint32 channel_number,
    //                Voltage min_value,
    //                Voltage max_value) override {}

private:

    MyRio& daq_;

};

}  // namespace mel

#endif  // MEL_QANALOGINPUT_HPP
