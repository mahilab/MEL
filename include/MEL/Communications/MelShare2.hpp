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

#ifndef MEL_MELSHARE2_HPP
#define MEL_MELSHARE2_HPP

#include <MEL/Config.hpp>
#include <MEL/Communications/SharedMemory.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Types.hpp>
#include <MEL/Utility/NamedMutex.hpp>
#include <string>
#include <vector>

namespace mel {

class Packet;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// High-level communication class that simplifies shared memory communication
template <typename T>
class MEL_API MelShare2 : NonCopyable {
public:
    /// Default constructor.
    MelShare2(const std::string& name, OpenMode mode = OpenOrCreate) :
        shm_(name, mode, 256),
        mutex_(name + "_mutex", mode)
    {

    }

    /// Writes a Packet to the MelShare
    void write(std::vector<T> data) {
        Lock lock(mutex_);
        uint32 size = static_cast<uint32>(data.size() * sizeof(T));
        shm_.write(&size, sizeof(uint32));
        shm_.write(&data[0], size, sizeof(uint32));
    }

    /// Reads a Packet from the MelShare
    std::vector<T> read() {
        Lock lock(mutex_);
        uint32 size = get_size();
        if (size > 0) {
            std::vector<T> data(size / sizeof(T));
            shm_.read(&data[0], size, sizeof(uint32));
            return data;
        }
        else
            return std::vector<T>();
    }

    /// Returns TRUE if the MelShare was successfully mapped on creation
    bool is_mapped() const {
        return shm_.is_mapped();
    }

private:
    /// Gets the number of bytes currently stored in the MelShare
    uint32 get_size() {
        uint32 size;
        shm_.read(&size, sizeof(uint32));
        return size;
    }

private:
    SharedMemory shm_;  ///< The memory mapped file for the data
    NamedMutex mutex_;  ///< The mutex guarding the memory map
};

}  // namespace mel

#endif  // MEL_MELSHARE2_HPP
