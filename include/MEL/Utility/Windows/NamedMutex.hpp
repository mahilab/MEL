#pragma once

#include <MEL/Utility/NonCopyable.hpp>
#include <string>
#include <memory>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Blocks concurrent access to shared resources from multiple processes
class NamedMutex : NonCopyable {

public:

    /// The mode by which a mutex is constructed
    enum Mode {
        OpenOrCreate, ///< create the named mutex if it does not exit
        OpenOnly      ///< only attempt to open an existing named mutex
    };

    /// Defaut constructor
    NamedMutex(std::string name, Mode mode = OpenOrCreate);

    /// Default destructor. Releases mutex if it is currently open.
    ~NamedMutex();

    /// Waits for mutex to release and attempts to lock it
    void lock();

    /// Releases lock on mutex
    void unlock();

private:

    class Impl;                   /// Pimpl idiom
    std::unique_ptr<Impl> impl_;  ///< OS-specific implementation
    std::string name_;            /// Name of the mutex

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
