# pragma once

#include <MEL/Utility/NonCopyable.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Blocks concurrent access to shared resources from multiple threads
class Mutex : NonCopyable {

public:

    /// Default constructor
    Mutex();

    /// Destructor
    ~Mutex();

    /// Lock the mutex
    void lock();

    /// Unlock the mutex
    void unlock();

private:

    class Impl;   /// Pimpl idiom
    Impl* impl_;  ///< OS-specific implementation

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::Mutex
/// \ingroup system
///
/// Mutex stands for "MUTual EXclusion". A mutex is a
/// synchronization object, used when multiple threads are involved.
///
/// When you want to protect a part of the code from being accessed
/// simultaneously by multiple threads, you typically use a
/// mutex. When a thread is locked by a mutex, any other thread
/// trying to lock it will be blocked until the mutex is released
/// by the thread that locked it. This way, you can allow only
/// one thread at a time to access a critical region of your code.
///
/// Usage example:
/// \code
/// Database database; // this is a critical resource that needs some protection
/// mel::Mutex mutex;
///
/// void thread1()
/// {
///     mutex.lock(); // this call will block the thread if the mutex is already locked by thread2
///     database.write(...);
///     mutex.unlock(); // if thread2 was waiting, it will now be unblocked
/// }
///
/// void thread2()
/// {
///     mutex.lock(); // this call will block the thread if the mutex is already locked by thread1
///     database.write(...);
///     mutex.unlock(); // if thread1 was waiting, it will now be unblocked
/// }
/// \endcode
///
/// Be very careful with mutexes. A bad usage can lead to bad problems,
/// like deadlocks (two threads are waiting for each other and the
/// application is globally stuck).
///
/// To make the usage of mutexes more robust, particularly in
/// environments where exceptions can be thrown, you should
/// use the helper class mel::Lock to lock/unlock mutexes.
///
/// melML mutexes are recursive, which means that you can lock
/// a mutex multiple times in the same thread without creating
/// a deadlock. In this case, the first call to lock() behaves
/// as usual, and the following ones have no effect.
/// However, you must call unlock() exactly as many times as you
/// called lock(). If you don't, the mutex won't be released.
///
/// \see mel::Lock
