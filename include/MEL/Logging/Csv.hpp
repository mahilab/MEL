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
#include <MEL/Logging/File.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Detail/StreamMeta.hpp>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace mel {

class MEL_API CsvRecord {

private:
    std::ostringstream  row_stream_; ///< stream
    mutable std::string row_string_; ///< string
};

/// Represents an instance of a Comma-Separated Value (CSV) file
class MEL_API Csv : public File {
public:


    /// Default constructor
    Csv();

    /// Constructor with filepath provided (opens file)
    Csv(const std::string& filepath, WriteMode w_mode = Truncate, OpenMode o_mode = OpenOrCreate);



public:

private:

    // hide functions inherited from File
    using File::unlink;
    using File::rename;
    using File::write;

};

// The following free functions are provided for convenience and are not
// intended to be used in conjunction with a Csv instance. These functions
// can be used to quickly read/write/append homogenous data to a CSV file.
// Since they open and close the file on demand, you should not use these
// to continously access a file in a loop; prefer an instance of the Csv
// class instead. Containers can be any type which overloads operator[]()
// for indexing, and provides a size() method for one or both dimensions.
// (e.g vector, array, vector<vector>, array<array>, vector<array>, etc.)

/// Reads a single row into a 1D container. The container must be presized.
template <typename Container1D>
bool csv_read_row(const std::string &filepath, Container1D &data_out, std::size_t row_offset, std::size_t col_offset = 0);

/// Reads multiple rows into a 1D container. The container must be presized.
template <typename Container2D>
bool csv_read_rows(const std::string &filepath, Container2D &data_out, std::size_t row_offset = 0, std::size_t col_offset = 0);

/// Writes a 1D container to file
template <typename Container1D>
bool csv_write_row(const std::string &filepath, const Container1D &data_in);

/// Writes a 2D container to file
template <typename Container2D>
bool csv_write_rows(const std::string &filepath, const Container2D &data_in);

/// Appends a 1D container to file
template <typename Container1D>
bool csv_append_row(const std::string &filepath, const Container1D &data_in);

/// Appends a 2D container to file
template <typename Container2D>
bool csv_append_rows(const std::string &filepath, const Container2D &data_in);

} // namespace mel

#include <MEL/Logging/Detail/Csv.inl>



