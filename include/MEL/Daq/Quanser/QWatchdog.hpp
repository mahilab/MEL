#pragma once

#include <MEL/Daq/Watchdog.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a hardware watchdog timer
class QWatchdog : public Watchdog {

public:

    /// Default constructor
    QWatchdog(QDaq& daq, Time timeout);

    /// Default destructor. Stops the watchdog if watching
    ~QWatchdog();

    bool start() override;

    bool kick() override;

    bool stop() override;

    bool is_expired() override;

    bool clear() override;

private:

    QDaq& daq_;  ///< Reference to parent QDaq

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
