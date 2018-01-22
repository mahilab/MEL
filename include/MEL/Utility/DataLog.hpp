#pragma once

#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <fstream>
#include <thread>
#include <algorithm>
#include <array>
#include <vector>
#include <string>
#include <tuple>
#include <atomic>
#include <iomanip>
#include <MEL/Utility/Clock.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Represents a floating point number format
enum class Format {
    Default,     ///< values are written with default notation
    Fixed,       ///< values are written with fixed notation
    Scientific   ///< values are written with scientific notion
};

template <typename ... Ts>
class DataLog {

public:

    /// Default constructor
    DataLog(bool autosave = true, std::size_t max_rows = 1000000) :
        saving_(false),
        log_saved_(true),
        max_rows_(max_rows),
        row_count_(0),
        col_count_(sizeof...(Ts)),
        format_(Format::Default),
        precision_(6),
        autosave_(autosave)
    {
        data_.reserve(max_rows);
        for (std::size_t i = 0; i < col_count_; ++i)
            column_names_[i] = "column" + stringify(i);
    }

    /// Constructor with custom column names
    DataLog(const std::array<std::string, sizeof...(Ts)>& column_names, bool autosave = true, std::size_t max_rows = 1000000) :
        DataLog(autosave, max_rows)
    {
        column_names_ = column_names;
    }

    /// Destructor. Waits for log to finish saving and performs autosave if reuqestd
    ~DataLog() {
        if (autosave_ && row_count_ > 0 && !log_saved_ && !saving_) {
            save_data("log", "autosaved_logs", true);
        }
        wait_for_save();
    }

    /// Sets the DataLog column names
    void set_column_names(const std::array<std::string, sizeof...(Ts)>& column_names) {
        column_names_ = column_names;
    }

    /// Adds a row of data from a tuple
    void add_row(const std::tuple<Ts...>& row) {
        Lock lock(mutex_);
        if (row_count_ == max_rows_)
            double_rows();
        data_.push_back(row);
        log_saved_ = false;
        row_count_ += 1;
    }

    /// Adds a row of data from a list of parameters
    void add_row(Ts... values) {
        add_row(std::make_tuple(values...));
    }

    /// Returns all data in a row as an std::tuple<Ts...>
    std::tuple<Ts...> get_row(std::size_t index) {
        Lock lock(mutex_);
        return data_[index];
    }

    /// Return all data in a column as std::vector<T>
    template <std::size_t I>
    std::vector<typename std::tuple_element<I, std::tuple<Ts...> >::type> get_col() {
        std::vector<typename std::tuple_element<I, std::tuple<Ts...> >::type> col(row_count_);
        for (std::size_t i = 0; i < row_count_; ++i) {
            col[i] = std::get<I>(data_[i]);
        }
        return col;
    }

    /// Returns the number of columns stored
    std::size_t get_col_count() {
        return col_count_;
    }

    /// Returns the number of rows stored
    std::size_t get_row_count() {
        return row_count_;
    }

    /// Saves all data collected so far to the to the specified location and filename.
    void save_data(const std::string& filename, const std::string& directory, bool timestamp = true) {
        saving_ = true;
        std::string full_filename;
        if (timestamp)
            full_filename = directory + get_path_slash() + filename + "_" + get_ymdhms() + ".csv";
        else
            full_filename = directory + get_path_slash() + filename + ".csv";
        print("Saving DataLog to <" + full_filename + ">.");
        std::thread t(&DataLog::save_thread_func, this, full_filename, directory, timestamp);
        t.detach();
    }

    /// Clears all data collected so far, but keeps the the column header names
    void clear_data() {
        data_.clear();
        data_.reserve(max_rows_);
        log_saved_ = true;
        row_count_ = 0;
    }

    /// Saves all data collected so far, the clears all data.
    void save_and_clear_data(std::string filename, std::string directory, bool timestamp = true) {
        save_data(filename, directory, timestamp);
        clear_data();
    }

    /// If the DataLog is currently saving, this function will block until it
    /// has completed.
    void wait_for_save() {
        if (saving_) {
            print("Waiting for DataLog to finish saving ... ", false);
            Lock lock(mutex_);
            print("Done");
        }
    }

    /// Sets the floating point number format the DataLog will be saved with.
    void set_format(Format format) {
        format_ = format;
    }

    ///< the floating point number precision the DataLog will be saved with
    void set_precision(std::size_t precision) {
        precision_ = precision;
    }

private:

    /// Doubles the number of reserved rows in data_
    void double_rows() {
        print("WARNING: DataLog max rows exceeded. Automatically doubling size. Increase max_rows in the constructor to avoid delays.");
        max_rows_ *= 2;
        data_.reserve(max_rows_);
    }

    /// Function called by saving thread
    void save_thread_func(const std::string& full_filename, const std::string& directory, bool timestamp) {
        Lock lock(mutex_);
        create_directory(directory);
        file_stream_.open(full_filename, std::ofstream::out | std::ofstream::trunc);
        file_stream_ << std::setprecision(precision_);
        if (format_ == Format::Fixed)
            file_stream_ << std::fixed;
        else if (format_ == Format::Scientific)
            file_stream_ << std::scientific;
        for (auto it = column_names_.begin(); it != column_names_.end(); ++it)
            file_stream_ << *it << ",";
        file_stream_ << std::endl;
        for (std::size_t i = 0; i < row_count_; i++) {
            file_stream_ << data_[i] << "," << std::endl;
        }
        file_stream_.close();
        log_saved_ = true;
        saving_ = false;
    }

private:

    Mutex mutex_;                 ///< Mutex used for safe file saving
    std::atomic<bool> saving_;    ///< true when the saving thread is running
    std::atomic<bool> log_saved_; ///< bool for tracking unsaved additions to log

    std::array<std::string, sizeof...(Ts)> column_names_; ///< vector of column names
    std::vector<std::tuple<Ts...>> data_;                 ///< data_[row][col]

    std::ofstream file_stream_;  ///< file stream

    std::size_t max_rows_;  ///< the estimated number of maximum rows that will be saved during the lifetime of the DataLog
    std::size_t row_count_;  ///< the row index at which the next row of data will be saved
    std::size_t col_count_;  ///< column count

    Format format_;          ///< the floating point number format the DataLog will be saved with
    std::size_t precision_;  ///< the floating point number precision the DataLog will be saved with

    bool autosave_;  ///< if true, the DataLog will be saved to a backup folder if deconstructed in an unsaved state

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
