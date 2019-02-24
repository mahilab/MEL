// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once


#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Types.hpp>
#include <string>
#include <vector>

namespace mel {

//==============================================================================
// TYPEDEFS
//==============================================================================

#if defined(_WIN32)
typedef void* MapHandle;
#else
typedef int MapHandle;
#endif

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates a unmanaged named memory map
class SharedMemory : NonCopyable {
public:

    /// Default constructor. Creates or opens a memory map containing size bytes
    SharedMemory(const std::string& name, OpenMode mode, std::size_t max_bytes = 256);

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

    /// Returns TRUE if the SharedMemory was successfully mapped on creation
    bool is_mapped() const;

private:
    /// Opens or creates a memory map
    static bool open_or_create(MapHandle& map, const std::string& name, std::size_t size);

    /// Opens a memory map if it exits
    static bool open_only(MapHandle& map, const std::string& name);

    /// Closes a memory map
    static void close(const std::string& name, MapHandle map);

    /// Maps a memory map buffer to the calling process's address space
    static void* map_buffer(MapHandle map, std::size_t size);

    /// Unmaps a memory map buffer from the calling process's address space
    static void unmap_buffer(void* buffer, std::size_t size);

private:
    const std::string name_;       ///< The name of the memory map file
    const std::size_t max_bytes_;  ///< The size of the mapped region in bytes
    MapHandle map_;                ///< OS specfic handle to the memory map
    void* buffer_;                 ///< The memory buffer of the map
    bool is_mapped_;               ///< true if the SharedMemory was successfully mapped
};

}  // namespace mel

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
