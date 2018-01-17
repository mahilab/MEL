#pragma once

#include <MEL/Utility/Lock.hpp>
#include <atomic>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Blocks concurrent access to shared resources from multiple threads
class Spinlock : public Lockable, NonCopyable {

public:

    /// Lock the Spinlock
    void lock() override;

    /// Unlock the Spinlock
    void unlock() override;

private:

    std::atomic_flag lock_ = ATOMIC_FLAG_INIT;  //< atomic flag lock

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
