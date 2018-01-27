// MIT License
//
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
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_EMGSIGNAL_HPP
#define MEL_EMGSIGNAL_HPP

#include <MEL/Daq/Input.hpp>
#include <MEL/Utility/RingBuffer.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class EmgSignal {

public:

    /// Constructor
    EmgSignal(AnalogInput::Channel channel, const std::size_t buffer_size);

    ///
    voltage get_sample();


private:

    AnalogInput::Channel channel_;
    RingBuffer<voltage> buffer_;

};

} // namespace mel

#endif // MEL_EMGSIGNAL_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
