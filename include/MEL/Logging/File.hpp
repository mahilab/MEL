// MIT License
//
// MEL - Mechatronics Library
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

#ifndef MEL_FILE_HPP
#define MEL_FILE_HPP

#include <MEL/Utility/NonCopyable.hpp>
#include <sstream>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class File : mel::NonCopyable {
public:
    /// Default constructor
    File();

    /// Cosntructor with file name provided
    File(const char* fileName);

    /// Default destructor
    ~File();

    /// Opens the file for input-output operations
    off_t open(const char* fileName);

    /// Writes to the file if the file is open
    int write(const void* buf, size_t count);

    /// Writes to the file if the file is open
    template <class CharType>
    int write(const std::basic_string<CharType>& str) {
        return write(str.data(), str.size() * sizeof(CharType));
    }

    off_t seek(off_t offset, int whence);

    /// Closes the file if the file is open
    void close();

    /// Removes the file if it exists
    static int unlink(const char* fileName);

    /// Renames the file if it exists
    static int rename(const char* oldFilename, const char* newFilename);

private:
    int m_file;  ///< Windows file type
};

}  // namespace mel

#endif  // MEL_FILE_HPP
