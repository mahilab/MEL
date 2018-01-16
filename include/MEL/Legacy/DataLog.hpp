#pragma once

#include <MEL/Utility/Types.hpp>
#include <vector>
#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class DataLog {

public:

    /// Default constructor
    DataLog(bool autosave = true, std::size_t num_rows = 1000000);

    /// Default deconstructor. Ensures saving completes.
    ~DataLog();

    /// Adds a column header to the DataLog (e.g. "Time" or "Joint Position")
    void add_col(const std::string& column_name);

    /// Adds multiple column headers to the DataLog
    void add_cols(const std::vector<std::string>& column_names);

    /// Adds a row of data to the DataLog. Data indices should match the order in which column headers were added. Call this inside or outside of any control loops.
    void add_row(const std::vector<double>& row_data);

    /// Returns the number of columns stored
    uint32 get_col_count();

    /// Returns the number of rows stored
    uint32 get_row_count();

    /// Returns all data in a column by column index
    std::vector<double> get_col(uint32 index);

    /// Returns all data in a column by column name
    std::vector<double> get_col(const std::string& column_name);

    /// Returns all data in a row
    std::vector<double> get_row(uint32 index);

    /// Clears all data collected so far, but keeps the the column header names
    void clear_data();

    /// Saves all data collected so far to the to the specified location and filename. Call this outside of any control loops.
    void save_data(std::string filename, std::string directory, bool timestamp = true);

    /// Saves all data collected so far, the clears all data.
    void save_and_clear_data(std::string filename, std::string directory, bool timestamp = true);

    /// Waits for saving to be completed
    void wait_for_save();

private:

    /// Doubles the number of maximum rows when max_rows_ is about to be eclipsed.
    void double_rows();

    /// Thread process in which data is saved to file
    void save_thread_function(std::string full_filename,
        std::vector<std::string> column_names,
        std::vector<std::vector<double>> data,
        uint32 num_rows,
        uint32 num_cols);

private:

    bool saving_;  ///< true if the DataLog is currently saving

    std::vector<std::string> column_names_; ///< vector of column names
    std::vector<std::vector<double>> data_; ///< data[cols][rows]

    std::size_t max_rows_; ///< the estimated number of maximum rows that will be saved during the lifetime of the DataLog
    std::size_t num_cols_; ///< column count
    uint32 row_index_;     ///< the row index at which the next row of data will be saved

    bool log_saved_; ///< bool for tracking unsaved additions to log
    bool autosave_;  ///< if true, the DataLog will be saved to a backup folder if deconstructed in an unsaved state

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

/// DataLog log;
///
/// log.add_cols({"Col_A", "Col_B", "Col_C"});
///
/// log.add_row({1, 2, 3});
/// log.add_row({4, 5, 6});
/// log.add_row({7, 8, 9});
///
/// log.save_and_clear_data("my_log0", "logs");
///
/// // logs/my_log0_2018-01-14-21.34.16.csv
/// //-------------------------------------
/// // Col_A,Col_B,Col_C,
/// // 1,2,3,
/// // 4,5,6,
/// // 7,8,9,
///
/// log.add_col("Col_D");
///
/// log.add_row({10, 11, 12, 13});
/// log.add_row({14, 15, 16, 17});
///
/// log.save_data("my_log1", "logs");
///
/// // logs/my_log1_2018-01-14-21.34.16.csv
/// //-------------------------------------
/// // Col_A,Col_B,Col_C,Col_D,
/// // 10,11,12,13,
/// // 14,15,16,17,
///
/// log.add_row({18, 19, 20, 21});
///
/// // data_log_backups/log_2018-01-14-21.34.16.csv
/// //-------------------------------------
/// // Col_A,Col_B,Col_C,Col_D,
/// // 10,11,12,13,
/// // 14,15,16,17,
/// // 18,19,20,21,
///
/// return 0;
