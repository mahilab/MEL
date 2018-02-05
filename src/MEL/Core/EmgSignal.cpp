#include <MEL/Core/EmgSignal.hpp>

namespace mel {

    EmgSignal::EmgSignal(AnalogInput::Channel channel, Filter demean_filter) :
    channel_(channel),
    demean_filter_(demean_filter)
{ }
//EmgSignal::EmgSignal(AnalogInput::Channel channel, const std::size_t buffer_size) :
//    channel_(channel),
//    buffer_(buffer_size)
//{}

Voltage EmgSignal::get_voltage() {
    //buffer_.push_back(channel_.get_value());
    return channel_.get_value();
}




} // namespace mel
