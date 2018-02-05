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
#include <MEL/Math/Filter.hpp>
//#include <MEL/Utility/RingBuffer.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class EmgSignal {

public:

    /// Constructor
    EmgSignal(AnalogInput::Channel channel, Filter demean_filter = Filter({ 0.814254556886246, -3.257018227544984,   4.885527341317476, -3.257018227544984,   0.814254556886246 }, { 1.000000000000000, -3.589733887112175,   4.851275882519415, -2.924052656162457,   0.663010484385890 })); // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
    //EmgSignal(AnalogInput::Channel channel, const std::size_t buffer_size);

    /// return the EMG voltage on the associated channel when channel was last updated
    voltage get_voltage();


private:

    AnalogInput::Channel channel_;
    Filter demean_filter_;
    //RingBuffer<voltage> buffer_;

};

} // namespace mel

#endif // MEL_EMGSIGNAL_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
