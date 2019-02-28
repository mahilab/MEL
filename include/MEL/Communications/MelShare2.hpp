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
 
 # pragma once

#include <MEL/Communications/SharedMemory.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Types.hpp>
#include <MEL/Utility/NamedMutex.hpp>
#include <string>
#include <array>
#include <vector>
#include <cassert>

namespace mel {

/// High-level shared memory for interprocess communication
template <typename T, std::size_t Size>
class MelShare2 : NonCopyable {
public:
    /// Default constructor.
    MelShare2(const std::string& name, OpenMode mode = OpenOrCreate) :
        shm_(name, mode, sizeof(uint32) + Size * sizeof(T)),
        mutex_(name + "_mutex", mode)
    { }

    /// Writes data to the MelShare
    void write(const std::vector<T>& data) {
        uint32 size = static_cast<uint32>(data.size());
        assert(size <= Size);
        Lock lock(mutex_);
        shm_.write(&size, sizeof(uint32));
        if (size > 0)
            shm_.write(&data[0], size * sizeof(T), sizeof(uint32));
    }

    /// Reads data from the MelShare
    void read(std::vector<T>& data) {
        uint32 size;
        Lock lock(mutex_);
        shm_.read(&size, sizeof(uint32));
        data.resize(size);
        if (size > 0)
            shm_.read(&data[0], size * sizeof(T), sizeof(uint32));
    }

    /// Returns TRUE if the MelShare was successfully mapped on creation
    bool mapped() const {
        return shm_.is_mapped();
    }

    /// Returns the current number of elements stored in memory
    std::size_t size() {
        uint32 size;
        Lock lock(mutex_);
        shm_.read(&size, sizeof(uint32));
        return static_cast<std::size_t>(size);
    }

private:
    SharedMemory shm_;  ///< The memory mapped file for the data
    NamedMutex mutex_;  ///< The mutex guarding the memory map
};

}  // namespace mel