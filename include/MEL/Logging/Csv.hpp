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
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace mel {

class MEL_API Csv : NonCopyable {
public:

    /// Open Mode
    enum Mode {
        Trunc = 0,  ///< existing content will be truncated on open
        Append = 1, ///< existing content will not be truncated on open 
    };

    /// Default constructor
    Csv();

    /// Constructor, automatically opens file
    Csv(const std::string& filepath, Mode mode = Trunc);

    /// Opens or creates file
    void open(const std::string& filepath, Mode mode = Trunc);

    /// Closes file
    void close();    

public:

    //=========================================================================
    // STATIC FUNCTIONS
    //=========================================================================

	/// Read a vector of vectors from a file
	static bool read(std::vector<std::vector<double>> &data_out, const std::string &filename, const std::string& directory = ".");

    /// Read a vector of vectors from a file with a row and column offset
    static bool read(std::vector<std::vector<double>>& data_out, std::size_t row_offset, std::size_t col_offset, const std::string &filename, const std::string& directory = ".");

    /// Writes a 1D container to file. The container must provide indexing and size().
    template <typename Container1D>
    static bool write_row(const std::string& filepath, const Container1D& data);

    /// Writes a 2D container to file. Both dimensions must provide indexing and size().
    template <typename Container2D>
    static bool write_rows(const std::string &filepath, const Container2D &data);

    /// Appends a 1D container to file. The container must provide indexing and size().
    template <typename Container1D>
    static bool append_row(const std::string &filepath, const Container1D &data);

    /// Appends a 2D container to file. Both dimensions must provide indexing and size().
    template <typename Container2D>
    static bool append_rows(const std::string& filepath, const Container2D& data);

private:

    /// Parses filepath into sub components
    static bool parse_filepath(const std::string &in, std::string &directory, std::string &filename, std::string &ext, std::string &full);

};

} // namespace mel

#include <MEL/Logging/Detail/Csv.inl>



