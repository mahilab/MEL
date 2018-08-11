// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
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

#ifndef MEL_SHAREDMEMORY_HPP
#define MEL_SHAREDMEMORY_HPP

#include <MEL/Config.hpp>
#include <MEL/Core/NonCopyable.hpp>
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
class MEL_API SharedMemory : NonCopyable {
public:
    /// Default constructor. Creates or opens a memory map containing size bytes
    SharedMemory(const std::string& name, std::size_t max_bytes = 256);

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
    static MapHandle create_or_open(const std::string& name, std::size_t size);

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
};

}  // namespace mel

#endif  // MEL_SHAREDMEMORY_HPP

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
