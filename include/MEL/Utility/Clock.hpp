#pragma once
#include <MEL/Utility/Time.hpp>

namespace mel
{

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Utility class that measures elapsed time.
class Clock {

public:

    /// Default constructor. Clock automatically starts on construction.
    Clock();

    /// Get the elapsed time since construction or last call to restart().
    Time get_elapsed_time() const;

    /// Restart the clock back to zero and return elapsed time since started.
    Time restart();

    /// Gets the time since epoch. Relative to nothing in particular.
    static Time get_current_time();

private:

    Time start_time_; ///< Time of last reset, in microseconds.

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::Clock
/// \ingroup utility
///
/// mel::Clock is a lightweight class for measuring time.
///
/// Its provides the most precise time that the underlying
/// OS can achieve (generally microseconds or nanoseconds).
/// It also ensures monotonicity, which means that the returned
/// time can never go backward, even if the system time is
/// changed.
///
/// Usage example:
/// \code
/// mel::Clock clock;
/// ...
/// Time time1 = clock.getElapsedTime();
/// ...
/// Time time2 = clock.restart();
/// \endcode
///
/// The mel::Time value returned by the clock can then be
/// converted to a number of seconds, milliseconds or even
/// microseconds.
///
/// \see mel::Time

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
