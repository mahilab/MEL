// MIT License
//
// MEL - MAHI Exoskeleton Library
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

#ifndef MEL_TIMER_HPP
#define MEL_TIMER_HPP

#include <MEL/Utility/Clock.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Utility class that creates a fixed rate waitable timer.
class Timer {

public:

    /// The waiting mode to be used when wait() is called.
    ///
    /// The WaitMode should be chosen wisely depending on the Timer period and
    /// the target platform. On Windows, the smallest amount of time a thread
    /// can be put to sleep is around 1 ms. Therefore, Sleep and Hybrid shouldn't
    /// be used on Windows for Timers running at 1000 Hz or higher; use Busy.
    /// On real-time Linux, threads can sleep for much smaller periods, so Sleep
    /// and Hybrid can be used reliably. Generally, using Hybrid over Sleep will
    /// be more accurate since Sleep can go over the requested sleep period.
    enum WaitMode {
        Busy,     ///< Waits 100% remaining time using a busy while loop
        Sleep,    ///< Waits 100% remaining time by sleeping the thread
        Hybrid,   ///< Waits 90% remaining time using Sleep, then 10% using Busy
        Hardware  ///< Waits according to user defined hardware implementation
    };

public:

    // Default constructor. Starts the Timer on construction.
    Timer(Time period, WaitMode mode = WaitMode::Busy);

    /// Restarts the Timer and returns the elapsed time
    Time restart();

    /// Waits the Timer and returns the elapsed time after the wait complete
    Time wait();

    /// Gets the elapsed time since construction or last call to restart().
    Time get_elapsed_time();

    /// Gets the ideal elapsed time since construction or last call to restart().
    /// Equal to the tick count times the Timer period
    Time get_elapsed_time_ideal();

    /// Gets the elapsed number of ticks since construction or the last call to restart().
    int64 get_elapsed_ticks();

    /// Gets the Timer period
    Time get_period();

protected:

    virtual void wait_hardware(const Time& duration);

private:

    /// Implements Busy wait mode
    void wait_busy(const Time& duration);

    /// Implements Sleep wait mode
    void wait_sleep(const Time& duration);

private:

    WaitMode mode_;  ///< The Timer's waiting mode
    Clock clock_;    ///< The Timer's internal clock
    Time period_;    ///< The Timer's waiting period
    int64 ticks_;    ///< The running tick count
    Time prev_time_; ///< Time saved at previous call to wait or restart

};

} // namespace mel

#endif // MEL_TIMER_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::Timer
/// \ingroup Utility
///
/// mel::Timer is a lightweight class for creating waitable fixed rate timers
///
/// You can use it to create fixed rate loops or waiting periods. It is built on
/// top of mel::Clock so it uses the most precise time that the underlying OS
/// can achieve and insures monotonicity. There are three waiting modes that can
/// be used: Busy, Sleep, and Hybrid. Busy is the most accurate, but uses 100%
/// of CPU time on the current thread. For high rate loops on Windows platforms
/// (>1000 Hz) this is the only accurate option. Sleep is the least accurate
/// option, but frees the CPU to the operating system for the entire wait period.
/// Use Sleep when accuracy is not critical. Hybrid combines the best of both,
/// Sleeping for the first 90% of the remaining wait time and Busy waiting for
/// the last 10%. Prefer Hybrid wait on real-time Linux operating systems.
///
/// Usage example:
/// \code
/// // create a 1000 Hz timer that uses a Busy waiting mode
/// mel::Timer my_timer(mel::milliseconds(1), Timer::WaitMode::Busy);
/// while(condition) {
///     // code that executes in less that 1 ms
///     ...
///     // wait the remaining time until 1 ms achieved
///     my_timer.wait();
/// }
/// ...
/// my_timer.restart();
/// while(my_timer.get_elapsed_time() < mel::seconds(10)) {
///     ...
///     my_timer.wait();
/// }
/// \endcode
///
/// Timer can also be used as a base class to wrap hardware clocks (i.e. cRIO)
/// by using WaitMode::Hardware and implementing the virtual wait_hardware()
/// function. For more information, see mel::cRIO.

/// \see mel::Clock, mel::Time

