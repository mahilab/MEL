#include <MEL/Core/Clock.hpp>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach/mach_time.h>
#else
#include <time.h>
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
// WINDOWS IMPLEMENTATION
//==============================================================================

#ifdef _WIN32

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

#elif __APPLE__

//==============================================================================
// APPLE IMPLEMENTATION
//==============================================================================

Time Clock::get_current_time() {
    static mach_timebase_info_data_t frequency = {0, 0};
    if (frequency.denom == 0)
        mach_timebase_info(&frequency);
    uint64 nanoseconds = mach_absolute_time() * frequency.numer / frequency.denom;
    return mel::microseconds(nanoseconds / 1000);
}

#else

//==============================================================================
// LINUX IMPLEMENTATION
//==============================================================================

Time Clock::get_current_time() {
    // POSIX implementation
    // https://linux.die.net/man/3/clock_gettime
    // https://forums.ni.com/t5/NI-Linux-Real-Time-Discussions/Help-to-solve-a-problem-with-C-on-cRIO-9068/td-p/3469892
    timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return mel::microseconds(static_cast<uint64>(time.tv_sec) * 1000000 + time.tv_nsec / 1000);
}

#endif

} // namespace mel

//==============================================================================
// APAPTED FROM: SFML (https://www.sfml-dev.org/)
//==============================================================================
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//==============================================================================
