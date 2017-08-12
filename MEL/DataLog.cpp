#include "DataLog.h"
#include <boost/filesystem.hpp>
#include <fstream>


namespace mel {

    DataLog::DataLog() :
        DataLog("no_name",1000000) {}

    DataLog::DataLog(std::string name, bool autosave, size_t max_rows) :
        name_(name),
        max_rows_(max_rows),
        num_cols_(0),
        row_index_(0),
        log_saved_(true),
        autosave_(autosave)
    {
    }

    DataLog::~DataLog() {
        if (!log_saved_ && autosave_) {
            save_data(name_, "data_backups", true);
        }
    }

    DataLog& DataLog::add_col(std::string column_name) {
        column_names_.push_back(column_name);
        data_.push_back(std::vector<double>(max_rows_, 0));
        log_saved_ = false;
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

    void DataLog::save_data(std::string filename, std::string directory, bool timestamp) {
        std::string full_filename = directory + "\\" + filename + "_" + get_ymdhms() + ".csv";
        boost::filesystem::path dir(directory.c_str());
        boost::filesystem::create_directories(dir);
        std::ofstream data_log;
        std::cout << "Saving DataLog <" << name_ << "> to <" << full_filename << "> ... ";
        data_log.open(full_filename, std::ofstream::out | std::ofstream::trunc);
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
        data_log.close();
        std::cout << "Done" << std::endl;
    }

    void DataLog::clear_data() {
        data_ = std::vector<std::vector<double>>();
        for (int i = 0; i < column_names_.size(); ++i)
            data_.push_back(std::vector<double>(max_rows_, 0));
        log_saved_ = true;
        row_index_ = 0;
    }

    void DataLog::save_and_clear_data(std::string filename, std::string directory, bool timestamp) {
        save_data(filename, directory, timestamp);
        clear_data();
    }
}