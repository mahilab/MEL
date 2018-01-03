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
