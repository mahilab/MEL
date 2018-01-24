#include <MEL/Core/EmgSignal.hpp>

namespace mel {

EmgSignal::EmgSignal(AnalogInput::Channel channel, Filter filter, std::size_t buffer_size) :
    channel_(channel),
    filter_(filter),
    buffer_(buffer_size)
{
    
}

voltage EmgSignal::get_unfiltered_sample() {
    return channel_.get_value();
}

voltage EmgSignal::get_filtered_sample() {

}


} // namespace mel
