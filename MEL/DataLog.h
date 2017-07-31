#pragma once
#include <string>
#include <vector>
#include "util.h"

namespace mel {

    class DataLog {

    public:

        /// Constructor
        DataLog();
        DataLog(std::string name, size_t num_rows = 1000000);
        ~DataLog();

        std::string name_;

        /// Adds a column header to the DataLog (e.g. "Time" or "Joint Position")
        DataLog& add_col(std::string column_name);
        /// Adds a row of data to the DataLog. Data indices should match the order in which column headers were added. Call this inside or outside of any control loops.
        void add_row(std::vector<double> row_data);
        /// Saves all data collected so far to the to the specificed location and filename. Call this outside of any control loops.
        void save_data(std::string directory);

    private:

        std::vector<std::string> column_names_;
        std::vector<std::vector<double>> data_;

        std::size_t max_rows_;
        std::size_t num_cols_;
        uint32 row_index_;

        bool log_saved_;
    };

}