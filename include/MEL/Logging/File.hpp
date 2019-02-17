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

#include <MEL/Config.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <sstream>

namespace mel {

/// Representats a file resource
class MEL_API File : NonCopyable {
public:
    /// Default constructor
    File();

    /// Constructor with filepath provided (opens file)
    File(const std::string& filepath);

    /// Default destructor (closes file if open)
    ~File();

    /// Opens the file for input-output operations
    off_t open(const std::string& filepath);

    /// Writes to the file if the file is open
    int write(const void* data, std::size_t count);

    /// Writes to the file if the file is open
    template <class CharType>
    int write(const std::basic_string<CharType>& str) {
        return write(str.data(), str.size() * sizeof(CharType));
    }

    /// Repositions file offset from beginning
    off_t seek_set(off_t offset);

    /// Repositions file offset from current
    off_t seek_cur(off_t offset);

    /// Repositions file offset from end
    off_t seek_end(off_t offset);

    /// Closes the file if the file is open
    void close();

public:

    /// Removes a file if it exists
    static int unlink(const std::string& filepath);

    /// Renames a file if it exists
    static int rename(const std::string& old_filename, const std::string& new_filename);

private:

    /// Repositions file offset according to directive whence and returns resulting offset
    off_t seek(off_t offset, int whence);

private:

    int m_file; ///< file handle
};

}  // namespace mel
