#pragma once

#include <MEL/Utility/NonCopyable.hpp>
#include <string>
#include <vector>

namespace mel {

//==============================================================================
// TYPEDEFS
//==============================================================================

#if defined(_WIN32)
    typedef void* MapHandle;
#elif __linux__
    typedef int MapHandle;
#endif

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a unmanaged named memory map
class SharedMemory : NonCopyable {

public:

    /// Default constructor. Creates or opens a memory map containing size bytes
    SharedMemory(const std::string& name, std::size_t max_size = 256);

    /// Default destructor. Closes the named memory map.
    ~SharedMemory();

    /// Copies #size bytes from #data to the memory map
    bool write(const void* data, std::size_t size, std::size_t offset = 0);

    /// Copies #size bytes from the memory map to #data
    bool read(void* data, std::size_t size, std::size_t offset = 0);

    /// Returns a pointer to the beginning of the underlying memory block/region
    void* get_address() const;

    /// Returns the string name of the named memory map
    std::string get_name() const;

private:

    /// Creates or opens a memory map
    static MapHandle create_or_open(std::string name, std::size_t size);

    /// Closes a memory map
    static void close(MapHandle map);

    /// Maps a memory map buffer to the calling process's address space
    static void* map_buffer(MapHandle map, std::size_t size);

    /// Unmaps a memory map buffer from the calling process's address space
    static void unmap_buffer(void* buffer);

private:

    const std::string name_;      ///< The name of the memory map file
    const std::size_t max_size_;  ///< The size of the mapped region in bytes
    MapHandle map_;               ///< OS specfic handle to the memory map
    void* buffer_;                ///< The memory buffer of the map

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// \class mel::SharedMemory
/// \ingroup Communications
///
/// ...
///
/// Usage example:
/// \code
/// SharedMemory map("my_map");
/// char msg1[8] = "abc";
/// map.write(msg1, 8);
/// ...
/// print((char*)map.get_address()); // ebc
/// ----------------------------------------------------------------------------
/// SharedMemory map("my_map");
/// ((char*)map.get_address())[0] = 'e';
/// \endcode
