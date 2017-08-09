#include "DataLog.h"
#include <boost/filesystem.hpp>
#include <fstream>


namespace mel {

    DataLog::DataLog() :
        DataLog("no_name",1000000) {}

    DataLog::DataLog(std::string name, size_t max_rows) :
        name_(name),
        max_rows_(max_rows),
        num_cols_(0),
        row_index_(0),
        log_saved_(true)
    {
    }

    DataLog::~DataLog() {
        if (!log_saved_) {
            save_data("backups");
        }
    }

    DataLog& DataLog::add_col(std::string column_name) {
        column_names_.push_back(column_name);
        data_.push_back(std::vector<double>(max_rows_, 0));
        num_cols_ += 1;
        return *this;
    }

    void DataLog::add_row(std::vector<double> row_data) {
        for (int i = 0; i < num_cols_; i++) {
            data_[i][row_index_] = row_data[i];
        }
        log_saved_ = false;
        row_index_ += 1;
    }

    void DataLog::save_data(std::string directory) {
        std::cout << "Saving DataLog <" << name_ << "> to directory <" << directory << "> ... ";
        std::string filename = directory + "\\" + name_ + "_" + get_current_date_time() + ".csv";
        mel::print(filename);
        boost::filesystem::path dir(directory.c_str());
        boost::filesystem::create_directories(dir);
        std::ofstream data_log;
        data_log.open(filename, std::ofstream::out | std::ofstream::trunc);
        for (auto it = column_names_.begin(); it != column_names_.end(); ++it) {
            data_log << *it << ",";
        }
        data_log << std::endl;

        for (int i = 0; i < row_index_; i++) {
            for (int j = 0; j < num_cols_; j++) {
                data_log << data_[j][i] << ",";
            }
            data_log << std::endl;
        }
        log_saved_ = true;
        std::cout << "Done" << std::endl;
    }


}