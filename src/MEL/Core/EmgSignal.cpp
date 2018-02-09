#include <MEL/Core/EmgSignal.hpp>


#include <MEL/Utility/Console.hpp> //debugging

namespace mel {

EmgSignal::EmgSignal(AnalogInput::Channel channel, Filter hp_filter, Filter lp_filter, Filter tkeo_lp_filter) :
channel_(channel),
hp_filter_(hp_filter),
lp_filter_(lp_filter),
tkeo_lp_filter_(tkeo_lp_filter)
{ }
//EmgSignal::EmgSignal(AnalogInput::Channel channel, const std::size_t buffer_size) :
//    channel_(channel),
//    buffer_(buffer_size)
//{}

void EmgSignal::update_signal() {
    //raw_voltage_ = channel_.get_value();
    //hp_filter_.filter(raw_voltage_, hp_signal_);
    //rect_signal_ = std::abs(hp_signal_);
    //lp_filter_.filter(rect_signal_, env_signal_);
    //tkeo_signal_ = tkeo_.tkeo(hp_signal_);
    //tkeo_rect_signal_ = std::abs(tkeo_signal_);
    //tkeo_lp_filter_.filter(tkeo_rect_signal_, tkeo_env_signal_);
}

void EmgSignal::reset_signal() {
    hp_filter_.reset();
    lp_filter_.reset();
    tkeo_.reset();
    tkeo_lp_filter_.reset();
    raw_voltage_ = 0.0;
    hp_signal_ = 0.0;
    rect_signal_ = 0.0;
    env_signal_ = 0.0;
    tkeo_signal_ = 0.0;
    tkeo_rect_signal_ = 0.0;
    tkeo_env_signal_ = 0.0;
}

Voltage EmgSignal::get_raw_voltage() {
    return channel_.get_value();
}

double EmgSignal::get_hp_signal() {
    return hp_signal_;
}

double EmgSignal::get_rect_signal() {
    return rect_signal_;
}

double EmgSignal::get_env_signal() {
    return env_signal_;
}

double EmgSignal::get_tkeo_signal() {
    return tkeo_signal_;
}

double EmgSignal::get_tkeo_rect_signal() {
    return tkeo_rect_signal_;
}

double EmgSignal::get_tkeo_env_signal() {
    return tkeo_env_signal_;
}

double EmgSignal::TeagerKaiserEngergyOperator::tkeo(const double x) {
    double y = s_[0] * s_[0] - x * s_[1];
    s_[0] = x;
    s_[1] = s_[0];
    return y;
}

void EmgSignal::TeagerKaiserEngergyOperator::reset() {
    s_ = std::vector<double>(n_, 0.0);
}

} // namespace mel
