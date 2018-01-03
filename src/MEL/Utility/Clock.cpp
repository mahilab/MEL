#include <MEL/Utility/Clock.hpp>
#ifdef __linux__
    #include <time.h>
#elif _WIN32
    #include <windows.h>
#endif

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Clock::Clock() :
    start_time_(Clock::get_current_time())
{
}

Time Clock::get_elapsed_time() const {
    return Clock::get_current_time() - start_time_;
}

Time Clock::restart() {
    Time now = Clock::get_current_time();
    Time elapsed = now - start_time_;
    start_time_ = now;
    return elapsed;
}

//==============================================================================
// LINUX IMPLEMENTATION
//==============================================================================

#ifdef __linux__

Time Clock::get_current_time() {
    // POSIX implementation
    // https://linux.die.net/man/3/clock_gettime
    timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return mel::microseconds(static_cast<uint64>(time.tv_sec) * 1000000 + time.tv_nsec / 1000);
}

//==============================================================================
// WINDOWS IMPLEMENTATION
//==============================================================================

#elif _WIN32

LARGE_INTEGER get_frequency() {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency;
}

Time Clock::get_current_time() {
    // Get the frequency of the performance counter
    // (it is constant across the program lifetime)
    static LARGE_INTEGER frequency = get_frequency();
    LARGE_INTEGER time;
   // Get the current time
    QueryPerformanceCounter(&time);
    // Return the current time as microseconds
    return mel::microseconds(1000000 * time.QuadPart / frequency.QuadPart);
}

#endif

} // namespace mel


