#include <MEL/Core/EmgSignal.hpp>

namespace mel {

EmgSignal::EmgSignal(AnalogInput::Channel channel, const std::size_t buffer_size) :
    channel_(channel),
    buffer_(buffer_size)
{
    
}

voltage EmgSignal::get_sample() {
    buffer_.push_back(channel_.get_value());
    return channel_.get_value();
}




} // namespace mel
