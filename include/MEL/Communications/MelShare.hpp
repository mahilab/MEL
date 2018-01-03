#pragma once

#include <MEL/Communications/SharedMemory.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/NonCopyable.hpp>
#include <MEL/Utility/Types.hpp>
#include <vector>
#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// High-level communication class that simplifies shared memory communication
class MelShare : NonCopyable {

public:

    /// Default constructor.
    MelShare(const std::string& name, std::size_t max_size = 256);

    /// Writes a vector of doubles to the MelShare
    void write_data(const std::vector<double>& data);

    /// Reads a vector of doubles from the MelShare
    std::vector<double> read_data();

    /// Writes a string message to the MelShare
    void write_message(const std::string& message);

    ///  Reads a string message from the MelShare
    std::string read_message();

private:

    /// Gets the number of bytes currently stored in the MelShare
    uint32 get_size();

private:

    SharedMemory shm_;  ///< The memory mapped file for the data
    Mutex mutex_;       ///< The mutex guarding the memory map

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
