#pragma once
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include "util.h"

namespace mel {

    class DataLog {

    public:

        DataLog(std::string directory, std::string filename, size_t num_rows = 1000000);

        DataLog& add_column(std::string column_name);
        void add_row(std::vector<double> row_data);

        std::vector<std::string> column_names_;
        std::vector<std::vector<double>> data_;

        void save_data();

        std::string   directory_;   // folder where data logs will be stored
        std::string   filename_;  // filename of the data log
        std::ofstream data_log_;  // stream for logging to the data log file 


        std::size_t max_rows_;
        std::size_t num_cols_;
        uint32 row_index_;

        bool log_started_;
    };

}