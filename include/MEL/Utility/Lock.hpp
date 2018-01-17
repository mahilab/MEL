#pragma once

#include <MEL/Utility/NonCopyable.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATIONS
//==============================================================================

/// Represents a lockable object (e.g. Mutex, NamedMutex, etc.)
class Lockable {

public:

    /// Virtual destructor
    virtual ~Lockable() {}

    /// Locks the Lockable
    virtual void lock() = 0;

    /// Unlocks the Lockable
    virtual void unlock() = 0;
};

/// RAII wrapper for automatically locking and unlocking Lockables
class Lock : NonCopyable {

public:

    /// Default contructor. The Lockable is automatically locked.
    Lock(Lockable& lockable);

    /// Destructor. The Lockable is automatically unlocked.
    ~Lock();

private:

    Lockable& lockable_;  ///< the lockable object
};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::Lock
/// \ingroup Utility
///
/// mel::Lock is a RAII wrapper for mel::Lockable, which provides
/// the basis for mel::Mutex, mel::NameMutex, and mel::Spinlock.
/// By unlocking the Lockable in its destructor, it ensures that
/// the Lockable will always be released when the current scope
/// (most likely a function) ends.
/// This is even more important when an exception or an early
/// return statement can interrupt the execution flow of the
/// function.
///
/// For maximum robustness, mel::Lock should always be used
/// to lock/unlock a Lockable.
///
/// Usage example:
/// \code
/// mel::Mutex mutex;
///
/// void function()
/// {
///     mel::Lock lock(mutex); // mutex is now locked
///
///     functionThatMayThrowAnException(); // mutex is unlocked if this function throws
///
///     if (someCondition)
///         return; // mutex is unlocked
///
/// } // mutex is unlocked
/// \endcode
///
/// Because the mutex is not explicitly unlocked in the code,
/// it may remain locked longer than needed. If the region
/// of the code that needs to be protected by the mutex is
/// not the entire function, a good practice is to create a
/// smaller, inner scope so that the lock is limited to this
/// part of the code.
///
/// \code
/// mel::Mutex mutex;
///
/// void function()
/// {
///     {
///       mel::Lock lock(mutex);
///       codeThatRequiresProtection();
///
///     } // mutex is unlocked here
///
///     codeThatDoesntCareAboutTheMutex();
/// }
/// \endcode
///
/// Having a mutex locked longer than required is a bad practice
/// which can lead to bad performances. Don't forget that when
/// a mutex is locked, other threads may be waiting doing nothing
/// until it is released.
///
/// \see mel::Mutex, mel::NamedMutex, mel::Spinlock
