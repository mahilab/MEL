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

/// Blocks concurrent access to shared resources from multiple processes
class NamedMutex : NonCopyable {

public:

    /// The mode by which a mutex is constructed
    enum Mode {
        Create, ///< create the named mutex if it does not exit
        Open    ///< only attempt to open an existing named mutex
    };

    /// Defaut constructor
    NamedMutex(std::string name, Mode mode = Create);

    /// Default destructor. Releases mutex if it is currently open.
    ~NamedMutex();

    /// Waits for mutex to release and attempts to lock it
    void lock();

    /// Releases lock on mutex
    void unlock();

private:

    static NamedMutexHandle create(std::string name);

    static NamedMutexHandle open(std::string name);

    static void close(NamedMutexHandle mutex);

    static void lock(NamedMutexHandle mutex);

    static void unlock(NamedMutexHandle mutex);

private:

    std::string name_;   /// Name of the mutex
    NamedMutexHandle mutex_;  /// Handle to underlying named mutex

    class Impl;
    Impl* impl_;

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
