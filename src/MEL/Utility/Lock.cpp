#include <MEL/Utility/Lock.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Lock::Lock(Lockable& lockable) :
    lockable_(lockable)
{
    lockable_.lock();
}

Lock::~Lock() {
    lockable_.unlock();
}

} // namespace mel
