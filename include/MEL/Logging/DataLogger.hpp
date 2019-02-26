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
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#pragma once

#include <MEL/Logging/File.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Logging/Table.hpp>
#include <string>
#include <vector>

namespace mel {

enum class WriterType {
    Buffered  = 0,  ///< stores data to write to file all at once later
    Immediate = 1,  ///< writes immediately to open file
};

enum class DataFormat {
    Default,    ///< values are written with default notation
    Fixed,      ///< values are written with fixed notation
    Scientific  ///< values are written with scientific notion
};


//==============================================================================
// CLASS DECLARATION
//==============================================================================

class DataLogger {
public:
    /// Constructor.
    DataLogger(WriterType writer_type = WriterType::Buffered,
               bool autosave          = true,
               size_t max_rows        = 1000000);

    /// Destructor. Waits for log to finish saving and performs autosave if
    /// requestd
    ~DataLogger();

    /// Store data record in a buffer to be written to a file later using
    /// save_data().
    void buffer(const std::vector<double>& data_record);

    /// Writes buffered data to the to the specified filename and directory.
    void save_data(const std::string& filename,
                   const std::string& directory = ".",
                   bool timestamp               = true);

    /// Clears all buffered data, but keeps the the column header names.
    void clear_data();

    /// Saves all data collected so far, the clears all data.
    void save_and_clear_data(std::string filename,
                             std::string directory = ".",
                             bool timestamp        = true);

    /// If the DataLog is currently saving, this function will block until it
    /// has completed.
    void wait_for_save();

    /// Write data record immediately to file (opens file if not already open),
    /// with no guarantee of execution time (generally 10s of us).
    void write(const std::vector<double>& data_record);

    /// Open the file for an Immediate writer type, clearing what is currently
    /// in the file.
    void open(const std::string& filename,
              const std::string& directory = ".",
              bool timestamp               = true);

    /// Open the file for an Immediate writer type, keeping what is currently in
    /// the file.
    void reopen(const std::string& filename,
                const std::string& directory = ".",
                bool timestamp               = true);

    /// Close the file for an Immediate writer type.
    void close();

    /// Set whether the file will be written to immediately or buffered
    void set_writer_type(WriterType writer_type);

    /// Set the header row, which contains the column names
    void set_header(const std::vector<std::string>& header);

    /// Set the data format and precision for every row of the data log
    void set_record_format(DataFormat data_format, std::size_t precision);

    /// Get the number of rows currently in the data buffer
    std::size_t get_row_count() const;

    /// Get the number of columns currently in the data buffer
    std::size_t get_col_count() const;

    /// Gets a row of data
    std::vector<double> get_row(std::size_t row);

    /// Gets a column of data
    std::vector<double> get_col(std::size_t col);

private:
    /// Writes the current header to the file
    void write_header();

    /// Converts a data record to a csv string with the specified formatting,
    /// i.e. format, precission
    std::string format(const std::vector<double>& data_record);

    /// Function called by saving thread
    void save_thread_func(const std::string& full_filename,
                          const std::string& directory,
                          std::vector<std::vector<double>> temp_data);

    /// Doubles the number of reserved rows in data_
    void double_rows();

private:
    WriterType writer_type_;///< stores whether writer is Immeadiate or Buffered
    Mutex mutex_;           ///< handles multi-threading
    File file_;             ///< File to which data will be written
    off_t file_size_;       ///< size of file_ since last open or write
    std::string file_ext_;  ///< file extension
    std::string filename_no_ext_;  ///< filename without extension
    bool file_opened_;             ///< true when file_ is open
    std::vector<std::vector<double>>  data_buffer_;  ///< storage container for log data
    bool autosave_;    ///< when true automatically saves the data log to a file upon destruction

    bool log_saved_;  ///< whether or not data log was successfully saved to file
    bool saving_;     ///< true when data is currently being saved to file
    std::vector<std::string> header_;  ///< vector of column names
    std::size_t max_rows_;   ///< number of rows reserved in the data buffer
    std::size_t row_count_;  ///< number of rows currently written to the data logger
    std::size_t col_count_;  ///< number of columns in the header
    DataFormat  format_;     ///< the floating point number format the DataLog will be saved with
    std::size_t precision_;  ///< the floating point number precision the DataLog will be saved with
};

}  // namespace mel
