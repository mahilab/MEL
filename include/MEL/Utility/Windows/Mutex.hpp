#pragma once

#include <MEL/Utility/NonCopyable.hpp>
#include <string>

namespace mel {

//==============================================================================
// TYPEDEFS
//==============================================================================

#if defined(_WIN32)
    typedef void* MutexHandle;
#elif __linux__
    typedef int MutexHandle;
#endif

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Named mutex for local and interprocess synchronization
class Mutex : NonCopyable {

public:

    /// The mode by which a mutex can be constructed.
    enum Mode {
        Create, ///< create the named mutex if it does not exit
        Open    ///< only attempt to open an existing named mutex
    };

    enum Status {
        LockAquired,    ///< mutex lock aquired
        LockAbandoned,  ///< wait on mutex lock abandoned
        LockTimeout,    ///< wait on mutex lock timed out
        LockFailed,     ///< wait on mutex lock failed
        NotOpen,        ///< mutex lock failed because mutex is not open
        ReleaseSuccess, ///< release of mutex lock succeeded
        ReleaseFailed,  ///< realse of mutex lock failed
        CloseSuccess,   ///< close of mutex succeeded
        CloseFailed     ///< close of mutex failed
    };

    /// Defaut constructor
    Mutex(std::string name, Mode mode = Create);

    /// Default destructor. Releases mutex if it is currently open.
    ~Mutex();

    /// Waits for mutex to release and attempts to lock it
    Status try_lock();

    /// Releases lock on mutex
    Status release();

private:

    static MutexHandle create(std::string name);

    static MutexHandle open(std::string name);

    static Status close(MutexHandle mutex);

    static Status try_lock(MutexHandle mutex);

    static Status release(MutexHandle mutex);

private:

    std::string name_;   /// Name of the mutex
    Mode mode_;          /// Mode by which this Mutex instance was created
    MutexHandle mutex_;  /// Handle to underlying named mutex
    bool has_lock_;      /// Indicates if this mutex has the lock

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
