#include <MEL/Core/Timer.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

static void wait_busy(const Time& duration) {
    Clock temp_clock;
    while (temp_clock.get_elapsed_time() < duration) {
        // do nothing
    }
}

static void wait_sleep(const Time& duration) {
    sleep(duration);
}

Timer::Timer(Frequency frequency, WaitMode mode) :
    Timer(frequency.to_time(), mode)
{
}

Timer::Timer(Time period, WaitMode mode) :
    mode_(mode),
    clock_(Clock()),
    period_(period),
    ticks_(0),
    prev_time_(Clock::get_current_time())
{
}

Timer::~Timer() { }

Time Timer::restart() {
    ticks_ = 0;
    prev_time_ = Clock::get_current_time();
    return clock_.restart();
}

Time Timer::wait() {
    Time remaining_time = period_ - (Clock::get_current_time() - prev_time_);
    if (remaining_time < Time::Zero) {
        LOG_IF(Verbose, ticks_ > 0) << "Timer with period " << period_ << " missed deadline by " << -remaining_time << " on tick number " << ticks_;
    }
    else {
        if (mode_ == WaitMode::Busy)
            wait_busy(remaining_time);
        else if (mode_ == WaitMode::Sleep)
            wait_sleep(remaining_time);
        else if (mode_ == WaitMode::Hybrid) {
            wait_sleep(remaining_time * 0.9);
            remaining_time = period_ - (Clock::get_current_time() - prev_time_);
            wait_busy(remaining_time);
        }
    }
    prev_time_ = Clock::get_current_time();
    ticks_ += 1;
    return get_elapsed_time();
}

Time Timer::get_elapsed_time_actual() {
    return clock_.get_elapsed_time();
}

Time Timer::get_elapsed_time() {
    return period_ * ticks_;
}

int64 Timer::get_elapsed_ticks() {
    return ticks_;
}

Frequency Timer::get_frequency() {
    return period_.to_frequency();
}

Time Timer::get_period() {
    return period_;
}



} // namespace mel
