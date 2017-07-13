#include "DataLog.h"
#include <boost/filesystem.hpp>

namespace mel {

    DataLog::DataLog(std::string directory, std::string filename, size_t max_rows) :
        directory_(directory),
        filename_(filename),
        max_rows_(max_rows),
        num_cols_(0),
        row_index_(0),
        log_started_(false)
    { }

    DataLog& DataLog::add_col(std::string column_name) {
        if (!log_started_) {
            column_names_.push_back(column_name);
            data_.push_back(std::vector<double>(max_rows_, 0));
            num_cols_ += 1;
        }
        else {
            std::cout << "WARNING: You are attempting to add a column to DataLog <" << filename_ << "> after logging as already begun" << std::endl;
        }
        return *this;
    }

    void DataLog::add_row(std::vector<double> row_data) {
        for (int i = 0; i < num_cols_; i++) {
            data_[i][row_index_] = row_data[i];
        }
        log_started_ = true;
        row_index_ += 1;
    }

    void DataLog::save_data() {
        std::cout << "Saving DataLog <" << filename_ << "> ... ";
        filename_ = directory_ + "\\clock_" + get_current_date_time() + ".csv";
        boost::filesystem::path dir(directory_.c_str());
        boost::filesystem::create_directory(dir);
        data_log_.open(filename_, std::ofstream::out | std::ofstream::trunc); 
        for (auto it = column_names_.begin(); it != column_names_.end(); ++it) {
            data_log_ << *it << ",";
        }
        data_log_ << std::endl;

        for (int i = 0; i < row_index_; i++) {
            for (int j = 0; j < num_cols_; j++) {
                data_log_ << data_[j][i] << ",";
            }
            data_log_ << std::endl;
        }
        std::cout << "Done" << std::endl;
    }


}