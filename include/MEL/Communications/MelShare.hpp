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

#ifndef MEL_MELSHARE_HPP
#define MEL_MELSHARE_HPP

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
class MEL_API MelShare : NonCopyable {
public:
    /// Default constructor.
    MelShare(const std::string& name, std::size_t max_bytes = 256);

    /// Writes a Packet to the MelShare
    void write(Packet& packet);

    /// Reads a Packet from the MelShare
    void read(Packet& packet);

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
    NamedMutex mutex_;  ///< The mutex guarding the memory map
};

}  // namespace mel

#endif  // MEL_MELSHARE_HPP
