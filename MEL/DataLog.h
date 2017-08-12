#pragma once
#include <string>
#include <vector>
#include "util.h"

namespace mel {

    class DataLog {

    public:

        /// Default constructor
        DataLog();
        /// Prefered constructor
        DataLog(std::string name, bool autosave = true, size_t num_rows = 1000000);
        ~DataLog();

        std::string name_; ///< DataLog name

        /// Adds a column header to the DataLog (e.g. "Time" or "Joint Position")
        DataLog& add_col(std::string column_name);
        /// Adds a row of data to the DataLog. Data indices should match the order in which column headers were added. Call this inside or outside of any control loops.
        void add_row(std::vector<double> row_data);
        /// Saves all data collected so far to the to the specificed location and filename. Call this outside of any control loops.
        void save_data(std::string filename, std::string directory, bool timestamp = true);
        /// Clears all data collected so far, but keeps the the column header names
        void clear_data();
        /// Saves all data collected so far, the clears all data.
        void save_and_clear_data(std::string filename, std::string directory, bool timestamp = true);

    private:

        std::vector<std::string> column_names_; 
        std::vector<std::vector<double>> data_;

        std::size_t max_rows_;
        std::size_t num_cols_;
        uint32 row_index_;

        bool log_saved_;
        bool autosave_;
    };

}