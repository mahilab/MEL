// MIT License
//
// MEL - MAHI Exoskeleton Library
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

#include <MEL/Communications/Windows/SharedMemory.hpp>
#include <MEL/Utility/Windows/NamedMutex.hpp>
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
    MelShare(const std::string& name, std::size_t max_bytes = 256);

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

} // namespace mel

#endif // MEL_MELSHARE_HPP


//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
