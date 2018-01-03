#pragma once
#include <MEL/Utility/System.hpp>
#include <vector>
#include <string>

namespace mel {

class DataLog {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    /// Default constructor
    DataLog();
    /// Prefered constructor
    DataLog(std::string name, bool autosave = true, std::size_t num_rows = 1000000);
    ~DataLog();

    //---------------------------------------------------------------------
    // PUBLIC VARIABLES
    //---------------------------------------------------------------------

    std::string name_;

    //---------------------------------------------------------------------
    // PUBLIC FUNCTIONS
    //---------------------------------------------------------------------

    /// Adds a column header to the DataLog (e.g. "Time" or "Joint Position")
    DataLog& add_col(std::string column_name);
    /// Adds a row of data to the DataLog. Data indices should match the order in which column headers were added. Call this inside or outside of any control loops.
    void add_row(std::vector<double> row_data);

    /// Returns the number of columns stored
    uint32 get_col_count();
    /// Returns the number of rows stored
    uint32 get_row_count();

    /// Returns all data in a column
    std::vector<double> get_col(uint32 index);
    std::vector<double> get_col(std::string column_name);
    /// Returns all data in a row
    std::vector<double> get_row(uint32 index);

     /// Saves all data collected so far to the to the specificed location and filename. Call this outside of any control loops.
    void save_data(std::string filename, std::string directory, bool timestamp = true);
    /// Clears all data collected so far, but keeps the the column header names
    void clear_data();
    /// Saves all data collected so far, the clears all data.
    void save_and_clear_data(std::string filename, std::string directory, bool timestamp = true);

    /// Waits for saving to be completed
    void wait_for_save();

private:

    //---------------------------------------------------------------------
    // PRIVATE FUNCTIONS
    //---------------------------------------------------------------------

    /// Doubles the number of maximum rows when max_rows_ is about to be eclipsed.
    /// This is function is a failsafe and will throw a warning message. If it is
    /// called, you should consider manually increasing the number of max rows for
    /// performance purposes.
    void double_rows();

    /// Thread process in which data is saved to file
    void save_thread_function(std::string full_filename, std::vector<std::string> column_names, std::vector<std::vector<double>> data, uint32 num_rows, uint32 num_cols);

    /// Creates a new directory if it does not exist.
    // void create_dir(std::string directory);

    //---------------------------------------------------------------------
    // PRIVATE VARIABLES
    //---------------------------------------------------------------------

    bool saving_ = false;

    std::vector<std::string> column_names_; ///< vector of column names
    std::vector<std::vector<double>> data_; ///< data[cols][rows]

    std::size_t max_rows_; ///< the estimated number of maximum rows that will be saved during the lifetim of the DataLog
    std::size_t num_cols_; ///< column count
    uint32 row_index_;     ///< the row index at which the next row of data will be saved

    bool log_saved_; ///< bool for tracking unsaved additions to log
    bool autosave_;  ///< if autosave is true, the DataLog will be saved to a backup folder if deconstructed in an unsaved state
};

} // namespace mel
