#pragma once

#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <fstream>
#include <thread>
#include <algorithm>
#include <array>
#include <vector>
#include <string>
#include <tuple>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

template <typename ... Ts>
class DataLog {

public:

    /// Default constructor
    DataLog(const std::array<std::string, sizeof...(Ts)>& column_names, bool autosave = true, std::size_t max_rows = 1000000) :
        column_names_(column_names),
        max_rows_(max_rows),
        num_cols_(sizeof...(Ts)),
        num_rows_(0),
        log_saved_(true),
        saving_(false),
        autosave_(autosave)
    {
        data_.reserve(max_rows);
    }

    /// Adds a row of data with a tuple
    void add_row(const std::tuple<Ts...>& row) {
        // if (num_rows_ == max_rows_)
        //     double_rows();
        data_.push_back(row);
        log_saved_ = false;
        num_rows_ += 1;
    }

    /// Adds a row of data with a list of parameters
    void add_row(Ts... values) {
        add_row(std::make_tuple(values...));
    }

    /// Returns all data in a row as an std::tuple
    std::tuple<Ts...> get_row(std::size_t index) {
        return data_[index];
    }

    /// Returns the number of columns stored
    std::size_t get_col_count() {
        return num_cols_;
    }

    /// Returns the number of rows stored
    std::size_t get_row_count() {
        return num_rows_;
    }

    /// Saves all data collected so far to the to the specified location and filename.
    void save_data(const std::string& filename, const std::string& directory, bool timestamp = true) {
        saving_ = true;
        std::string full_filename;
        if (timestamp)
            full_filename = directory + get_path_slash() + filename + "_" + get_ymdhms() + ".csv";
        else
            full_filename = directory + get_path_slash() + filename + ".csv";
        create_directory(directory);
        print("Saving DataLog to <" + full_filename + ">.");
        std::ofstream file_stream;
        file_stream.open(full_filename, std::ofstream::out | std::ofstream::trunc);
        for (auto it = column_names_.begin(); it != column_names_.end(); ++it)
            file_stream << *it << ",";
        file_stream << std::endl;
        for (std::size_t i = 0; i < num_rows_; i++) {
            file_stream << data_[i] << "," << std::endl;
        }
        file_stream.close();
        saving_ = false;
        log_saved_ = true;
    }

    /// Clears all data collected so far, but keeps the the column header names
    void clear_data() {
        data_.clear();
        data_.reserve(max_rows_);
        log_saved_ = true;
        num_rows_ = 0;
    }

    /// Saves all data collected so far, the clears all data.
    void save_and_clear_data(std::string filename, std::string directory, bool timestamp = true) {
        save_data(filename, directory, timestamp);
        clear_data();
    }

private:

    /// Doubles the number of reserved rows
    void double_rows() {
        print("WARNING: DataLog max rows exceeded. Automatically doubling size. Increase max_rows in the constructor to avoid delays.");
        max_rows_ *= 2;
        data_.reserve(max_rows_);
    }

private:

    bool saving_;  ///< true if the DataLog is currently saving

    std::array<std::string, sizeof...(Ts)> column_names_; ///< vector of column names
    std::vector<std::tuple<Ts...>> data_;                 ///< data_[row][col]

    std::size_t max_rows_;  ///< the estimated number of maximum rows that will be saved during the lifetime of the DataLog
    std::size_t num_rows_;  ///< the row index at which the next row of data will be saved
    std::size_t num_cols_;  ///< column count

    bool log_saved_; ///< bool for tracking unsaved additions to log
    bool autosave_;  ///< if true, the DataLog will be saved to a backup folder if deconstructed in an unsaved state

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
