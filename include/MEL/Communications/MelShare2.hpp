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

#include <MEL/Communications/SharedMemory.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Core/Types.hpp>
#include <MEL/Utility/NamedMutex.hpp>
#include <string>
#include <array>

namespace mel {

/// High-level shared memory for interprocess communication
template <typename T, std::size_t Size>
class MelShare2 : NonCopyable {
public:
    /// Default constructor.
    MelShare2(const std::string& name, OpenMode mode = OpenOrCreate) :
        shm_(name, mode, Size * sizeof(T)),
        mutex_(name + "_mutex", mode)
    { }

    /// Writes data to the MelShare
    void write(const std::array<T, Size>& data_in) {
        Lock lock(mutex_);
        shm_.write(&data_in[0], Size * sizeof(T), 0);
    }

    /// Reads data from the MelShare
    void read(std::array<T, Size>& data_out) {
        Lock lock(mutex_);
        shm_.read(&data_out[0], Size * sizeof(T), 0);
    }

    /// Returns TRUE if the MelShare was successfully mapped on creation
    bool is_mapped() const {
        return shm_.is_mapped();
    }

private:
    SharedMemory shm_;  ///< The memory mapped file for the data
    NamedMutex mutex_;  ///< The mutex guarding the memory map
};

}  // namespace mel

#endif  // MEL_MELSHARE2_HPP
