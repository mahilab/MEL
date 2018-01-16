#pragma once

#include <MEL/Utility/NonCopyable.hpp>
#include <string>

namespace mel {

//==============================================================================
// TYPEDEFS
//==============================================================================

#if defined(_WIN32)
    typedef void* NamedMutexHandle;
#elif __linux__
    typedef int NamedMutexHandle;
#endif

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Named mutex for local and interprocess synchronization
class NamedMutex : NonCopyable {

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
    NamedMutex(std::string name, Mode mode = Create);

    /// Default destructor. Releases mutex if it is currently open.
    ~NamedMutex();

    /// Waits for mutex to release and attempts to lock it
    Status lock();

    /// Releases lock on mutex
    Status unlock();

private:

    static NamedMutexHandle create(std::string name);

    static NamedMutexHandle open(std::string name);

    static Status close(NamedMutexHandle mutex);

    static Status lock(NamedMutexHandle mutex);

    static Status unlock(NamedMutexHandle mutex);

private:

    std::string name_;   /// Name of the mutex
    Mode mode_;          /// Mode by which this NamedMutex instance was created
    NamedMutexHandle mutex_;  /// Handle to underlying named mutex
    bool has_lock_;      /// Indicates if this mutex has the lock

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
