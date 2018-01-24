#pragma once
#include <MEL/Math/Filter.hpp>
#include <MEL/Daq/Input.hpp>
#include <MEL/Utility/RingBuffer.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class EmgSignal {

public:

    /// Constructor
    EmgSignal(AnalogInput::Channel channel, Filter filter, std::size_t buffer_size);

    ///
    voltage get_unfiltered_sample();

    voltage get_filtered_sample();



private:

    AnalogInput::Channel channel_;

    RingBuffer<voltage> buffer_;

    Filter filter_;

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
