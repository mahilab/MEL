#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Timer::Timer(Time period, WaitMode mode) :
    mode_(mode),
    clock_(Clock()),
    period_(period),
    ticks_(0),
    prev_time_(Clock::get_current_time())
{
}

Time Timer::restart() {
    ticks_ = 0;
    prev_time_ = Clock::get_current_time();
    return clock_.restart();
}

Time Timer::wait() {
    Time remaining_time = period_ - (Clock::get_current_time() - prev_time_);
    if (remaining_time < Time::Zero) {
        remaining_time = Time::Zero; // deadline missed!
    }
    if (mode_ == WaitMode::Busy)
        wait_busy(remaining_time);
    else if (mode_ == WaitMode::Sleep)
        wait_sleep(remaining_time);
    else if (mode_ == WaitMode::Hybrid) {
        wait_sleep(remaining_time * 0.9);
        remaining_time = period_ - (Clock::get_current_time() - prev_time_);
        wait_busy(remaining_time);
    }
    ticks_ += 1;
    prev_time_ = Clock::get_current_time();
    return clock_.get_elapsed_time();
}

Time Timer::get_elapsed_time() {
    return clock_.get_elapsed_time();
}

Time Timer::get_elapsed_time_ideal() {
    return period_ * ticks_;
}

int64 Timer::get_elapsed_ticks() {
    return ticks_;
}

Time Timer::get_period() {
    return period_;
}

void Timer::wait_hardware(const Time& duration) {
    wait_busy(duration);
}

void Timer::wait_busy(const Time& duration) {
    Time start = Clock::get_current_time();
    while ((Clock::get_current_time() - start) < duration) {
    }
}

void Timer::wait_sleep(const Time& duration) {
    sleep(duration);
}



} // namespace mel
