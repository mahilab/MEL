#include <MEL/Daq/Watchdog.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINTIONS
//==============================================================================

Watchdog::Watchdog(Time timeout) :
    timeout_(timeout),
    watching_(false)
{
}

Watchdog::~Watchdog() {

}

void Watchdog::set_timeout(Time timeout) {
    timeout_ = timeout;
}

} // namespace mel
